#ifndef LLVM_TRANSFORMS_UTILS_LIQUIDPARSER_REFINEMENTPARSER_H
#define LLVM_TRANSFORMS_UTILS_LIQUIDPARSER_REFINEMENTPARSER_H

#include <string>
#include <vector>
#include <map>
#include <set>

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

	class RefinementPiece
	{
	public:
		std::string RefinementPieceText;
		std::set<std::string> VariablesUsed;
	};

	class ParsedRefinement
	{
	public:
		std::string OriginalRefinementString;
		std::string ParsedRefinementString;

		ParsedRefinement ReplaceVariables(const std::map<std::string, std::string>& replacements) const;
		std::vector<RefinementPiece> GetRefinementParts() const;
	};
	
	class RefinementParser
	{
	public:
		static bool ParseRefinement(const std::string refinement, std::vector<ParserError>& errors, ParsedRefinement& output) noexcept;
	};
}

#endif