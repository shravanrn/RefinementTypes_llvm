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


	class RefinementGrammarRewriteVisitor : public RefinementGrammarVisitor
	{
	public:
		TokenStreamRewriter* Rewriter;
		const std::map<std::string, std::string>& VariableReplacements;

		//somehow taking the TokenStreamRewriter by reference causes the variable to reset its state after the visitor is run
		//not looking into it for now, but seems like a bug with the antlr generated parser
		RefinementGrammarRewriteVisitor(TokenStreamRewriter* rewriter, const std::map<std::string, std::string>& variableReplacements) : Rewriter(rewriter), VariableReplacements(variableReplacements) {}

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

		antlrcpp::Any visitVariable(RefinementGrammarParser::VariableContext *context) override {
			const std::string currVariableName = context->getText();

			auto pos = VariableReplacements.find(currVariableName);
			if (pos != VariableReplacements.end())
			{
				std::string replacement = pos->second;

				//get the first variable token
				tree::TerminalNode* node = context->getToken(RefinementGrammarLexer::VARIABLE, 0);
				if (node == nullptr) {
					throw new std::exception("Could not retrieve the node for variable");
				}

				Token* token = node->getSymbol();
				if (token == nullptr) {
					throw new std::exception("Could not retrieve the token for variable");
				}

				Rewriter->replace(token, replacement);
			}

			return visitChildren(context);
		}

		antlrcpp::Any visitValueExpression(RefinementGrammarParser::ValueExpressionContext *context) override {
			return visitChildren(context);
		}
		antlrcpp::Any visitSingleConstraint(RefinementGrammarParser::SingleConstraintContext *context) override {
			return visitChildren(context);
		}
		antlrcpp::Any visitDisjunctions(RefinementGrammarParser::DisjunctionsContext *context) override {
			return visitChildren(context);
		}
		antlrcpp::Any visitConjunctiveNormalForm(RefinementGrammarParser::ConjunctiveNormalFormContext *context) override {
			return visitChildren(context);
		}
		antlrcpp::Any visitParse(RefinementGrammarParser::ParseContext *context) override {
			return visitChildren(context);
		}
	};

	bool parseAndRewrite(const std::string refinement, const std::map<std::string, std::string>& variableReplacements, std::vector<ParserError>& errors, std::string& output) noexcept
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

			TokenStreamRewriter rewriter(&tokens);
			RefinementGrammarRewriteVisitor generator(&rewriter, variableReplacements);

			auto parsedData = parser.parse();

			if (errorHandler.Errors.size() > 0)
			{
				//errors found
				return false;
			}

			generator.visit(parsedData);

			output = rewriter.getText();

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
	
	const std::string ParsedRefinement::ReplaceVariables(const std::map<std::string, std::string>& replacements) const
	{
		//At this stage, we will assume parsing is already successfull, so just supress any errors
		std::vector<ParserError> errors;
		std::string output;
		bool success = parseAndRewrite(this->OriginalRefinementString, replacements, errors, output);
		assert(success == true && errors.size() == 0);
		return output;
	}

	bool RefinementParser::ParseRefinement(const std::string refinement, std::vector<ParserError>& errors, ParsedRefinement& output) noexcept
	{
		//no replacements
		std::map<std::string, std::string> replacements;
		output.OriginalRefinementString = refinement;
		return parseAndRewrite(refinement, replacements, errors, output.ParsedRefinementString);
	}
};