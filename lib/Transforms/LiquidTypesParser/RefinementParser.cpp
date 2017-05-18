#include "ANTLRInputStream.h"
#include "BaseErrorListener.h"
#include "RefinementParser.h"
#include "RefinementGrammarBaseVisitor.h"
#include "RefinementGrammarLexer.h"

using namespace antlr4;
using namespace liquid;
using namespace std::literals::string_literals;

namespace {

	class LexerParserErrorHandler : public BaseErrorListener
	{
	public:
		std::vector<ParserError>& Errors;

		LexerParserErrorHandler(std::vector<ParserError>& errors) : Errors(errors) {}

		void syntaxError(IRecognizer *recognizer,
			Token * offendingSymbol,
			size_t line,
			size_t charPositionInLine,
			const std::string &msg, std::exception_ptr e) override
		{
			Errors.push_back(ParserError(msg, line, charPositionInLine));
		}
	};


	class RefinementGrammarFixpointRewriteVisitor : public RefinementGrammarBaseVisitor
	{
	private:
		TokenStreamRewriter* Rewriter;

	public:
		//somehow taking the TokenStreamRewriter by reference causes the variable to reset its state after the visitor is run
		//not looking into it for now, but seems like a bug with the antlr generated parser
		RefinementGrammarFixpointRewriteVisitor(TokenStreamRewriter* rewriter) : Rewriter(rewriter) {}

		antlrcpp::Any visitBinaryoperator(RefinementGrammarParser::BinaryoperatorContext *context) override {
			//C++ uses % for modulo, whereas fixpoint uses mod
			if (context->getText() == "%"s)
			{
				//get the first binary operator token
				tree::TerminalNode* node = context->getToken(RefinementGrammarLexer::BINARYOPERATOR, 0);
				if (node == nullptr) {
					throw new std::exception("Could not retrieve the node for % binary operator");
				}

				Token* token = node->getSymbol();
				if (token == nullptr) {
					throw new std::exception("Could not retrieve the token for % binary operator");
				}

				Rewriter->replace(token, "mod"s);
			}

			return visitChildren(context);
		}
	};

	class RefinementVariableRewriteVisitor : public RefinementGrammarBaseVisitor
	{
	public:
		TokenStreamRewriter* Rewriter;
		const std::map<std::string, std::string>& VariableReplacements;

		//somehow taking the TokenStreamRewriter by reference causes the variable to reset its state after the visitor is run
		//not looking into it for now, but seems like a bug with the antlr generated parser
		RefinementVariableRewriteVisitor(TokenStreamRewriter* rewriter, const std::map<std::string, std::string>& variableReplacements) : Rewriter(rewriter), VariableReplacements(variableReplacements) {}

		antlrcpp::Any visitVariable(RefinementGrammarParser::VariableContext *context) override {
			const std::string currVariableName = context->getText();

			auto pos = VariableReplacements.find(currVariableName);
			if (pos != VariableReplacements.end())
			{
				std::string newVariableName = pos->second;

				//get the first variable token
				tree::TerminalNode* node = context->getToken(RefinementGrammarLexer::VARIABLE, 0);
				if (node == nullptr) {
					throw new std::exception("Could not retrieve the node for variable");
				}

				Token* token = node->getSymbol();
				if (token == nullptr) {
					throw new std::exception("Could not retrieve the token for variable");
				}

				Rewriter->replace(token, newVariableName);
			}

			return visitChildren(context);
		}
	};

	class RefinementDisjunctionCollectVisitor : public RefinementGrammarBaseVisitor
	{
	public:
		std::vector<RefinementPiece>& Disjunctions;

		//somehow taking the TokenStreamRewriter by reference causes the variable to reset its state after the visitor is run
		//not looking into it for now, but seems like a bug with the antlr generated parser
		RefinementDisjunctionCollectVisitor(std::vector<RefinementPiece>& disjunctions) : Disjunctions(disjunctions) {}

		antlrcpp::Any visitDisjunctions(RefinementGrammarParser::DisjunctionsContext *context) override {

			auto is = context->start->getInputStream();
			misc::Interval interval(context->start->getStartIndex(), context->stop->getStopIndex());
			std::string originalText = is->getText(interval);

			RefinementPiece refinementPiece;
			refinementPiece.RefinementPieceText = originalText;
			Disjunctions.push_back(refinementPiece);

			return visitChildren(context);
		}

		antlrcpp::Any visitVariable(RefinementGrammarParser::VariableContext *context) override {

			assert(Disjunctions.size() > 0);
			const std::string currVariableName = context->getText();
			Disjunctions[Disjunctions.size() - 1].VariablesUsed.insert(currVariableName);

			return visitChildren(context);
		}
	};

	template<typename FnRetType>
	bool safeParse(const std::string refinement, 
		const std::function<FnRetType(CommonTokenStream&, ParserRuleContext*)>& functionToCall,
		std::vector<ParserError>& errors, 
		FnRetType& output
	) noexcept
	{
		try
		{
			ANTLRInputStream s(refinement);
			RefinementGrammarLexer lexer(&s);
			CommonTokenStream tokens(&lexer);
			RefinementGrammarParser parser(&tokens);

			LexerParserErrorHandler errorHandler(errors);
			parser.removeErrorListeners();
			parser.addErrorListener(&errorHandler);

			ParserRuleContext* parsedData = parser.parse();

			if (errorHandler.Errors.size() > 0)
			{
				//errors found
				return false;
			}

			output = functionToCall(tokens, parsedData);
			return errorHandler.Errors.size() == 0;
		}
		catch (unsigned int param)
		{
			std::string errorMessage = "Parser Error: Error Code -"s + std::to_string(param);
			errors.push_back(ParserError(errorMessage, 0, 0));
		}
		catch (int param)
		{
			std::string errorMessage = "Parser Error: Error Code -"s + std::to_string(param);
			errors.push_back(ParserError(errorMessage, 0, 0));
		}
		catch (const char* &param)
		{
			std::string errorMessage = "Parser Error: "s + param;
			errors.push_back(ParserError(errorMessage, 0, 0));
		}
		catch (const std::string& param)
		{
			std::string errorMessage = "Parser Error: "s + param;
			errors.push_back(ParserError(errorMessage, 0, 0));
		}
		catch (std::exception& param)
		{
			std::string errorMessage = "Parser Error: "s + param.what();
			errors.push_back(ParserError(errorMessage, 0, 0));
		}
		catch (...)
		{
			std::string errorMessage = "Parser Error: Unknown error"s;
			errors.push_back(ParserError(errorMessage, 0, 0));
		}

		return false;
	}
}

namespace liquid
{
	ParsedRefinement ParsedRefinement::ReplaceVariables(const std::map<std::string, std::string>& replacements) const
	{
		const std::function<std::string(CommonTokenStream&, ParserRuleContext*)> parseFunction = 
			[&replacements](CommonTokenStream& tokens, ParserRuleContext* parsedData) -> std::string {
				TokenStreamRewriter tokenRewriter(&tokens);
				RefinementVariableRewriteVisitor variableRewriter(&tokenRewriter, replacements);
				variableRewriter.visit(parsedData);

				const std::string ret = tokenRewriter.getText();
				return ret;
			};

		std::vector<ParserError> errors;

		std::string variableReplacedString;
		{
			//At this stage, we will assume parsing is already successfull, so just suppress any errors
			bool success = safeParse(this->OriginalRefinementString, parseFunction, errors, variableReplacedString);
			assert(success);
		}

		ParsedRefinement ret;
		{
			//again, we should not see any errors anymore as they would be caught earlier
			bool success = RefinementParser::ParseRefinement(variableReplacedString, errors, ret);
			assert(success);
		}

		return ret;
	}

	std::vector<RefinementPiece> ParsedRefinement::GetRefinementParts() const
	{
		std::vector<RefinementPiece> disjunctions;

		const std::function<bool(CommonTokenStream&, ParserRuleContext*)> parseFunction =
			[&disjunctions](CommonTokenStream& tokens, ParserRuleContext* parsedData) -> bool {

			RefinementDisjunctionCollectVisitor disjunctionCollector(disjunctions);
			disjunctionCollector.visit(parsedData);
			return true;
		};

		std::vector<ParserError> errors;

		{
			bool dummyRet;
			//At this stage, we will assume parsing is already successful, so just suppress any errors
			bool success = safeParse(this->OriginalRefinementString, parseFunction, errors, dummyRet);
			assert(success);
		}

		for(auto& disjunction : disjunctions)
		{
			//we now reparse every individial disjunction
			ParsedRefinement parsedDisjunction;
			bool success = RefinementParser::ParseRefinement(disjunction.RefinementPieceText, errors, parsedDisjunction);
			assert(success);
			disjunction.RefinementPieceText = parsedDisjunction.ParsedRefinementString;
		}

		return disjunctions;
	}

	bool RefinementParser::ParseRefinement(const std::string refinement, std::vector<ParserError>& errors, ParsedRefinement& output) noexcept
	{
		const std::function<std::string(CommonTokenStream&, ParserRuleContext*)> parseFunction = 
			[](CommonTokenStream& tokens, ParserRuleContext* parsedData) -> std::string {
				TokenStreamRewriter tokenRewriter(&tokens);
				RefinementGrammarFixpointRewriteVisitor fixpointRewriter(&tokenRewriter);
				fixpointRewriter.visit(parsedData);

				const std::string ret = tokenRewriter.getText();
				return ret;
			};

		output.OriginalRefinementString = refinement;
		return safeParse(refinement, parseFunction, errors, output.ParsedRefinementString);
	}
};