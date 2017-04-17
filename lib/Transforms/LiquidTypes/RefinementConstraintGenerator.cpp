#include "llvm/Transforms/LiquidTypes/RefinementConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"

using namespace std::literals::string_literals;

namespace liquid {

	std::string RefinementConstraintGenerator::getMaxValueForIntWidth(int width)
	{
		std::string maxVal = APInt::getMaxValue(width).toString(10 /* radix */, false /* unsigned */);
		return maxVal;
	}

	std::vector<std::string> RefinementConstraintGenerator::getNonDependentConstraints(std::string variableConstraint)
	{
		//TODO:
		std::vector<std::string> ret{ variableConstraint };
		return ret;
	}

	ResultType RefinementConstraintGenerator::registerAndRetrieveIntegerQualifiers(const llvm::IntegerType& type, std::vector<std::string>& constraints)
	{
		std::string maxIntVal = getMaxValueForIntWidth(type.getIntegerBitWidth());
		constraints.push_back("__value >= 0"s);
		constraints.push_back("__value <= "s + maxIntVal);

		std::string constraintName = "Int" + std::to_string(type.getBitWidth()) + "Limit";

		unsigned int i = 0;
		for (auto& constraint : constraints)
		{
			auto addQualRes = constraintBuilder.AddQualifierIfNew(constraintName + std::to_string(i), { FixpointBaseType::INT }, { "__value" }, constraint);
			if (!addQualRes.Succeeded) { return addQualRes; }
			i++;
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::addConstraintsForVariable(const RefinementMetadataForVariable& variable, const std::string& prefix, const std::string& blockName, const bool ignoreAssumes)
	{
		std::vector<std::string> variableConstraints;
		FixpointBaseType fixpointType;

		auto convertResult = fixpointTypeConvertor.GetFixpointType(*(variable.LLVMType), fixpointType);
		if (!convertResult.Succeeded) { return convertResult; }

		if (variable.LLVMType->isIntegerTy())
		{
			auto intType = dyn_cast<llvm::IntegerType>(variable.LLVMType);
			auto intQualRes = registerAndRetrieveIntegerQualifiers(*intType, variableConstraints);
			if (!intQualRes.Succeeded) { return intQualRes; }
		}
		else
		{
			return ResultType::Error("Refinement types: non int type not yet supported");
		}

		if (!ignoreAssumes)
		{
			if (variable.Assume != "")
			{
				std::vector<std::string> nonDependentConstraints = getNonDependentConstraints(variable.Assume);
				variableConstraints = RefinementUtils::vectorAppend(variableConstraints, nonDependentConstraints);
			}
		}

		if (variable.Verify != "")
		{
			std::vector<std::string> nonDependentConstraints = getNonDependentConstraints(variable.Verify);
			variableConstraints = RefinementUtils::vectorAppend(variableConstraints, nonDependentConstraints);
		}

		variableEnv.AddVariable(blockName, prefix + variable.LLVMName);
		auto createBinderRes = constraintBuilder.CreateBinderWithQualifiers(prefix + variable.LLVMName, fixpointType, variableConstraints);
		if (!createBinderRes.Succeeded) { return createBinderRes; }

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::buildConstraintsFromSignatureForBlock(const RefinementMetadata& refinementData, const std::string& prefix, const std::string& blockName, const bool ignoreParameterAssumes, const bool ignoreReturnAssumes)
	{
		for (auto& param : refinementData.Parameters)
		{
			//function parameters are not declared in any block, but are instead a part of the signature
			//we will just add these to the entry block, which is the first block of any function
			ResultType addConstraintRet = addConstraintsForVariable(param, prefix, blockName, ignoreParameterAssumes /* ignoreAssumes */);
			if (!addConstraintRet.Succeeded)
			{
				return addConstraintRet;
			}
		}

		//assumes of the return type can be ignored here (it will be used at call sites of this functions) 
		//as there is nothing to check and we can't use this information to help verification of this function
		ResultType addConstraintRet = addConstraintsForVariable(refinementData.Return, prefix, blockName, ignoreReturnAssumes /* ignoreAssumes */);
		if (!addConstraintRet.Succeeded)
		{
			return addConstraintRet;
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::BuildConstraintsFromSignature(const RefinementMetadata& refinementData)
	{
		return buildConstraintsFromSignatureForBlock(refinementData, "" /* prefix */, "entry", false /* ignoreParameterAssumes */, true /* ignoreReturnAssumes */);
	}

	ResultType RefinementConstraintGenerator::generateCallSignatureVariables(const std::string& blockName, const CallInst& callInst, const AnalysisRetriever& analysisRetriever, std::string& prefixUsed, const RefinementFunctionInfo* &callFunctionInfo)
	{
		auto callFnName = callInst.getCalledFunction();

		if (!analysisRetriever.ContainsAnalysisForFunction(*callFnName))
		{
			return ResultType::Error("Refinement Types: Not all function calls are supported currently");
		}

		callFunctionInfo = analysisRetriever.GetAnalysisForFunction(*callFnName);

		auto opRegisterName = callInst.getName().str();
		prefixUsed = opRegisterName + "_";
		return buildConstraintsFromSignatureForBlock(callFunctionInfo->ParsedFnRefinementMetadata, prefixUsed, blockName, true /* ignoreParameterAssumes */, false /* ignoreReturnAssumes */);
	}

	ResultType RefinementConstraintGenerator::BuildConstraintsFromInstructions(const RefinementMetadata& refinementData, const AnalysisRetriever& analysisRetriever)
	{
		for (auto& block : Func)
		{
			auto blockName = block.getName().str();
			for (auto& instr : block)
			{
				if (auto binaryOpInst = dyn_cast<BinaryOperator>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureBinaryOperatorConstraint(blockName, *binaryOpInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto returnInst = dyn_cast<ReturnInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureReturnInstructionConstraint(blockName, *returnInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto cmpInst = dyn_cast<llvm::CmpInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureComparisonInstructionConstraint(blockName, *cmpInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto zextInst = dyn_cast<llvm::ZExtInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureZeroExtendInstructionConstraint(blockName, *zextInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto branchInst = dyn_cast<BranchInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureBranchInstructionConstraint(blockName, *branchInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto phiInst = dyn_cast<PHINode>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CapturePhiInstructionConstraint(blockName, *phiInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto selectInst = dyn_cast<SelectInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureSelectInstructionConstraint(blockName, *selectInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto callInst = dyn_cast<CallInst>(&instr))
				{
					std::string prefixUsed;
					const RefinementFunctionInfo* callRefFunctionInfo;
					{
						ResultType res = generateCallSignatureVariables(blockName, *callInst, analysisRetriever, prefixUsed, callRefFunctionInfo);
						if (!res.Succeeded) { return res; }
					}

					ResultType res = instructionConstraintBuilder.CaptureCallInstructionConstraint(blockName, *callInst, prefixUsed, callRefFunctionInfo);
					if (!res.Succeeded) { return res; }
				}
				else
				{
					return ResultType::Error("Unknown instruction type "s + instr.getOpcodeName());
				}
			}
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::CaptureLoopConstraints(const llvm::LoopInfo& loopInfo)
	{
		for (auto& block : Func)
		{
			auto currentLoopDepth = loopInfo.getLoopDepth(&block);
			if (currentLoopDepth > 0)
			{
				ResultType addQualRes = constraintBuilder.AddQualifierIfNew("loopStepQualifier", { FixpointBaseType::INT, FixpointBaseType::INT, FixpointBaseType::INT }, { "v", "a", "b" }, "(v - a) mod b == 0");
				if (!addQualRes.Succeeded) { return addQualRes; }
			}
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::ToString(std::string& output)
	{
		return constraintBuilder.ToStringOrFailure(output);
	}
}