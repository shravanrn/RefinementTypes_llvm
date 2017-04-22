#ifndef LLVM_TRANSFORMS_UTILS_LIQUIDPARSER_REFINEMENTPARSER_H
#define LLVM_TRANSFORMS_UTILS_LIQUIDPARSER_REFINEMENTPARSER_H

#include <string>
#include <vector>
#include <map>

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

	class ParsedRefinement
	{
	public:
		std::string OriginalRefinementString;
		std::string ParsedRefinementString;

		const std::string ReplaceVariables(const std::map<std::string, std::string>& replacements) const;
	};
	
	class RefinementParser
	{
	public:
		static bool ParseRefinement(const std::string refinement, std::vector<ParserError>& errors, ParsedRefinement& output) noexcept;
	};
}

#endif