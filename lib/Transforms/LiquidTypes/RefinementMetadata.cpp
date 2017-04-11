#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"

using namespace std::string_literals;
using namespace liquid;
using namespace llvm;

namespace {
	RefinementMetadataForVariable parseVariable(RefinementMetadataForVariable_Raw original)
	{
		RefinementMetadataForVariable copy;
		copy.OriginalName = original.OriginalName;
		copy.LLVMName = original.LLVMName;
		copy.OriginalType = original.OriginalType;
		copy.LLVMType = original.LLVMType;
		copy.Assume = original.Assume;
		copy.Verify = original.Verify;
		return copy;
	}
}

ResultType RefinementMetadata::ParseMetadata(RefinementMetadata_Raw& in, RefinementMetadata& out)
{
	for (auto const& original : in.Qualifiers)
	{
		auto copy = original;
		out.Qualifiers.push_back(copy);
	}

	for (auto const& original : in.Parameters)
	{
		out.Parameters.push_back(parseVariable(original));
	}

	out.Return = parseVariable(in.Return);
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