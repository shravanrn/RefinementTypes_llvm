#include "llvm/IR/Function.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include <vector>
#include <string>
#include <map>
#include <memory>

using namespace std::string_literals;
using namespace liquid;
using namespace llvm;

ResultType RefinementMetadata::IncrementPointerAndGetString(const MDOperand* &operand, const MDOperand* operandEnd, std::string& str)
{
	if (operand != operandEnd)
	{
		operand++;
		MDString* metadataType_Str = dyn_cast<MDString>(operand->get());

		if (!metadataType_Str)
		{
			return ResultType::Error("Refinement types: unexpected type of metadata type node");
		}

		str = metadataType_Str->getString().str();
		return ResultType::Success();
	}

	return ResultType::Error("Refinement types: Expected more parameters");
}

ResultType RefinementMetadata::ParseRefinement(RefinementMetadata& refinementData, const MDOperand* operand, const MDOperand* operandEnd)
{
	MDString* metadataType_Str = dyn_cast<MDString>(operand->get());

	if (!metadataType_Str)
	{
		return ResultType::Error("Refinement types: unexpected type of metadata type node");
	}

	std::string metadataType = metadataType_Str->getString().str();
	if (metadataType == "qualifier")
	{
		std::string qualifier;
		auto qualResult = IncrementPointerAndGetString(operand, operandEnd, qualifier);
		if (!qualResult.Succeeded) { return qualResult; }

		refinementData.Qualifiers.push_back(qualifier);
	}
	else if (metadataType == "signature_return" || metadataType == "signature_parameter")
	{
		std::string originalType, originalName;
		auto originalTypeResult = IncrementPointerAndGetString(operand, operandEnd, originalType);
		if (!originalTypeResult.Succeeded) { return originalTypeResult; }

		auto originalNameResult = IncrementPointerAndGetString(operand, operandEnd, originalName);
		if (!originalNameResult.Succeeded) { return originalNameResult; }

		if (metadataType == "signature_return")
		{
			refinementData.Return.OriginalType = originalType;
			refinementData.Return.OriginalName = originalName;
		}
		else
		{
			RefinementMetadataForVariable paramRefinement;
			paramRefinement.OriginalType = originalType;
			paramRefinement.OriginalName = originalName;
			refinementData.Parameters.push_back(paramRefinement);
		}
	}
	else if (metadataType == "assume" || metadataType == "verify")
	{
		RefinementMetadataForVariable* latest = &(refinementData.Return);
		if (refinementData.Parameters.size() > 0)
		{
			latest = &(refinementData.Parameters[refinementData.Parameters.size() - 1]);
		}

		std::string assumeVerifyString;
		auto assumeVerifyResult = IncrementPointerAndGetString(operand, operandEnd, assumeVerifyString);
		if (!assumeVerifyResult.Succeeded) { return assumeVerifyResult; }

		if (metadataType == "assume")
		{
			latest->Assume = assumeVerifyString;
		}
		else
		{
			latest->Verify = assumeVerifyString;
		}
	}
	else
	{
		return ResultType::Error("Refinement types: Unexpected metadata type " + metadataType);
	}

	std::string endString;
	auto endResult = IncrementPointerAndGetString(operand, operandEnd, endString);
	if (!endResult.Succeeded) { return endResult; }

	if (endString != "end")
	{
		return ResultType::Error("Refinement types: Expected 'end' string. Got " + endString);
	}

	return ResultType::Success();
}

ResultType RefinementMetadata::Extract(Function& F, RefinementMetadata& ret)
{
	MDNode* metadata = F.getMetadata("refinement");
	//no refinement data
	//if (metadata == nullptr)
	//{
	//	return nullptr;
	//}

	auto operandCount = metadata->getNumOperands();

	if (operandCount % 2 != 0)
	{
		return ResultType::Error("Refinement types: unexpected metadata node count");
	}

	for (auto operand = metadata->op_begin(); operand != metadata->op_end(); operand++)
	{
		auto parseResult = ParseRefinement(ret, operand, metadata->op_end());
		if (!parseResult.Succeeded)
		{
			return parseResult;
		}
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