#include "ANTLRInputStream.h"
#include "BaseErrorListener.h"
#include "RefinementParser.h"
#include "RefinementGrammarBaseVisitor.h"
#include "RefinementGrammarLexer.h"

using namespace antlr4;
using namespace std::literals::string_literals;

namespace liquid
{
	class LexerParserErrorHandler : public BaseErrorListener
	{
	public:
		std::vector<ParserError>& Errors;

		LexerParserErrorHandler(std::vector<ParserError>& errors) : Errors(errors) {}

		virtual void syntaxError(IRecognizer *recognizer,
			Token * offendingSymbol,
			size_t line,
			size_t charPositionInLine,
			const std::string &msg, std::exception_ptr e) override
		{
			Errors.push_back(ParserError(msg, line, charPositionInLine));
		}
	};


	class RefinementGrammarAbstractVisitor : public RefinementGrammarBaseVisitor
	{

	};

	bool RefinementParser::ParseRefinement(std::string refinement, std::vector<ParserError>& errors, std::string& output) noexcept
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

			auto parsedData = parser.conjunctiveNormalForm();
			RefinementGrammarAbstractVisitor generator;
			antlrcpp::Any parsedStructure = generator.visit(parsedData);

			output = refinement;
			return errorHandler.Errors.size() > 0;
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
		catch (char* &param) 
		{ 
			std::string errorMessage = "Parser Error: "s + param;
			errors.push_back(ParserError(errorMessage, 0, 0));
		}
		catch (std::string& param)
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

};