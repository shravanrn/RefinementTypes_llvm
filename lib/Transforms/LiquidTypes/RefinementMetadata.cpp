#include "llvm/IR/Metadata.h"
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
namespace
{
	class RefinementMetadataKVP
	{
	public:
		std::string Key;
		std::string Value;

		static ResultType Get(Metadata* metadataType, Metadata* metadataDetail, RefinementMetadataKVP& out)
		{
			MDString* metadataType_Str = dyn_cast<MDString>(metadataType);

			if (!metadataType_Str)
			{
				return ResultType::Error("Refinement types: unexpected type of metadata type node");
			}

			MDString* metadataDetail_Str = dyn_cast<MDString>(metadataDetail);

			if (!metadataDetail_Str)
			{
				return ResultType::Error("Refinement types: unexpected type of metadata detail node");
			}

			out.Key = metadataType_Str->getString().str();
			out.Value = metadataDetail_Str->getString().str();

			return ResultType::Success();
		}

		static ResultType SplitByColon(std::string signature, RefinementMetadataKVP& out)
		{
			std::size_t position = signature.find(":"s);
			if (position == std::string::npos)
			{
				return ResultType::Error("Could not find the location of the `:` in metadata string - " + signature);
			}

			out.Key = signature.substr(0, position);
			out.Value = signature.substr(position + 1, signature.length() - (position + 1));

			return ResultType::Success();
		}
	};

	ResultType getMetadataKVPs(MDNode* metadata, std::vector<RefinementMetadataKVP>& metadataKVPs)
	{
		auto operandCount = metadata->getNumOperands();

		std::vector<Metadata*> refinementMetadataNodes;
		for (auto operand = metadata->op_begin(); operand != metadata->op_end(); operand++)
		{
			refinementMetadataNodes.push_back(operand->get());
		}

		for (unsigned int i = 0; i < operandCount; i += 2)
		{
			RefinementMetadataKVP operandKvp;
			ResultType getKvpResult = RefinementMetadataKVP::Get(refinementMetadataNodes[i], refinementMetadataNodes[i + 1], operandKvp);

			if (!getKvpResult.Succeeded)
			{
				return ResultType::Error(getKvpResult.ErrorMsg + " Operand num " + std::to_string(i));
			}

			metadataKVPs.push_back(operandKvp);
		}

		return ResultType::Success();
	}

	void extractLLVMDetails(Function& F, RefinementMetadata& ret)
	{
		ret.Return.LLVMName = "return";
		ret.Return.LLVMType = F.getReturnType();

		std::vector<llvm::Argument*> args;
		for each(auto& arg in F.args())
		{
			args.push_back(&arg);
		}

		for (unsigned int i = 0; i < args.size(); i++)
		{
			ret.Parameters[i].LLVMName = args.at(i)->getName().str();
			ret.Parameters[i].LLVMType = args.at(i)->getType();
		}
	}
}

ResultType RefinementMetadata::Extract(Function& F, RefinementMetadata& ret)
{
	MDNode* metadata = F.getMetadata("refine");
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

	std::vector<RefinementMetadataKVP> refinementMetadataNodes;
	auto getMetadataResult = getMetadataKVPs(metadata, refinementMetadataNodes);

	if (!getMetadataResult.Succeeded)
	{
		return ResultType::Error(getMetadataResult.ErrorMsg);
	}

	std::map<std::string, RefinementMetadataForVariable*> metadataMapping;

	for (auto const& metadataKVP : refinementMetadataNodes)
	{
		if (metadataKVP.Key == "signature_return" || metadataKVP.Key == "signature_parameter")
		{
			RefinementMetadataKVP kvp;
			auto splitResult = RefinementMetadataKVP::SplitByColon(metadataKVP.Value, kvp);

			RefinementMetadataForVariable* target = &(ret.Return);
			if (metadataKVP.Key == "signature_parameter")
			{
				RefinementMetadataForVariable parameter;
				ret.Parameters.push_back(parameter);
				target = &(ret.Parameters[ret.Parameters.size() - 1]);
			}

			if (!splitResult.Succeeded)
			{
				return ResultType::Error("Refinement types: error parsing refinement - " + splitResult.ErrorMsg);
			}

			target->OriginalName = kvp.Key;
			target->OriginalType = kvp.Value;
			metadataMapping[target->OriginalName] = target;
		}
	}

	extractLLVMDetails(F, ret);

	for (auto const& metadataKVP : refinementMetadataNodes)
	{
		if (metadataKVP.Key == "signature_return" || metadataKVP.Key == "signature_parameter") {}
		else if (metadataKVP.Key == "qualifier")
		{
			ret.Qualifiers.push_back(metadataKVP.Value);
		}
		else if (metadataKVP.Key == "assume" || metadataKVP.Key == "verify")
		{
			RefinementMetadataKVP kvp;
			auto splitResult = RefinementMetadataKVP::SplitByColon(metadataKVP.Value, kvp);

			if (!splitResult.Succeeded)
			{
				return ResultType::Error("Refinement types: could not find parameter name in - " + metadataKVP.Value);
			}

			if (RefinementUtils::containsKey(metadataMapping, kvp.Key))
			{
				RefinementMetadataForVariable* curr = metadataMapping[kvp.Key];
				if (metadataKVP.Key == "assume")
				{
					curr->Assume = kvp.Value;
				}
				else
				{
					curr->Verify = kvp.Value;
				}
			}
			else
			{
				return ResultType::Error("Refinement types: could not find parameter with name - " + kvp.Key);
			}
		}
		else
		{
			return ResultType::Error("Refinement types: unknown metadata type - " + metadataKVP.Key);
		}
	}

	return ResultType::Success();
}
