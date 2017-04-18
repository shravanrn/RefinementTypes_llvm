#ifndef LLVM_TRANSFORMS_UTILS_LIQUIDPARSER_REFINEMENTPARSER_H
#define LLVM_TRANSFORMS_UTILS_LIQUIDPARSER_REFINEMENTPARSER_H

#include <string>
#include <vector>

namespace liquid
{
	class ParserError
	{
	public:
		std::string Message;
		unsigned int Line;
		unsigned int Column;

		ParserError(std::string message, unsigned int line, unsigned int column) : Message(message), Line(line), Column(column) {}
	};
	
	class RefinementParser
	{
	public:
		static bool ParseRefinement(std::string refinement, std::vector<ParserError>& errors, std::string& output) noexcept;
	};
}

#endif