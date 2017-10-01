#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidFixpointBuilder/RefinementUtils.h"

#include <vector>

using namespace std::string_literals;
using namespace liquid;
using namespace llvm;

namespace {
	std::vector<ParserError> parseVariable(RefinementMetadataForVariable_Raw original, RefinementMetadataForVariable& copy)
	{
		copy.OriginalName = original.OriginalName;
		copy.LLVMName = original.LLVMName;
		copy.OriginalType = original.OriginalType;
		copy.LLVMType = original.LLVMType;

		std::vector<ParserError> parseErrors;
		RefinementParser::ParseRefinement(original.Assume, parseErrors, copy.Assume);
		RefinementParser::ParseRefinement(original.Verify, parseErrors, copy.Verify);

		return parseErrors;
	}

	void appendErrorString(ParserError error, std::string& message)
	{
		message +=
			//"Line: "s + std::to_string(error.Line) + 
			" Column: "s + std::to_string(error.Column) +
			" "s + error.Message + "\n"s;
	}
}

ResultType RefinementMetadata::ParseMetadata(RefinementMetadata_Raw& in, RefinementMetadata& out)
{
	for (auto const& original : in.Qualifiers)
	{
		auto copy = original;
		out.Qualifiers.push_back(copy);
	}
	
	bool foundErrors = false;
	std::string errorString = "";

	for (auto const& original : in.Parameters)
	{
		RefinementMetadataForVariable copy;
		std::vector<ParserError> errors = parseVariable(original, copy);

		if (errors.size() > 0)
		{
			for (auto& error : errors) { appendErrorString(error, errorString);}
			foundErrors = true;
		}

		out.Parameters.push_back(copy);
	}

	{
		std::vector<ParserError> errors = parseVariable(in.Return, out.Return);

		if (errors.size() > 0)
		{
			for (auto& error : errors) { appendErrorString(error, errorString); }
			foundErrors = true;
		}
	}

	if (foundErrors)
	{
		return ResultType::Error(errorString);
	}

	return ResultType::Success();
}

RefinementMetadata& RefinementMetadata::operator=(const RefinementMetadata& other) {

	for (auto const& original : other.Qualifiers)
	{
		auto copy = original;
		Qualifiers.push_back(copy);
	}

	for (auto const& original : other.Parameters)
	{
		auto copy = original;
		Parameters.push_back(copy);
	}

	Return = other.Return;
	return *this;
}

RefinementMetadataForVariable& RefinementMetadataForVariable::operator=(const RefinementMetadataForVariable& other)
{
	OriginalName = other.OriginalName;
	LLVMName = other.LLVMName;
	OriginalType = other.OriginalType;
	LLVMType = other.LLVMType;
	Assume = other.Assume;
	Verify = other.Verify;
	return *this;
}
