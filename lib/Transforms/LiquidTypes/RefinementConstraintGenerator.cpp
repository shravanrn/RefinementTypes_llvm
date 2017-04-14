#include "llvm/Transforms/LiquidTypes/RefinementConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"

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

	ResultType RefinementConstraintGenerator::addConstraintsForVariable(const RefinementMetadataForVariable& variable, const std::string& blockName, bool ignoreAssumes)
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

		variableEnv.AddVariable(blockName, variable.LLVMName);
		auto createBinderRes = constraintBuilder.CreateBinderWithQualifiers(variable.LLVMName, fixpointType, variableConstraints);
		if (!createBinderRes.Succeeded) { return createBinderRes; }

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::BuildConstraintsFromSignature(const RefinementMetadata& refinementData)
	{
		for (auto& param : refinementData.Parameters)
		{
			//function parameters are not declared in any block, but are instead a part of the signature
			//we will just add these to the entry block, which is the first block of any function
			ResultType addConstraintRet = addConstraintsForVariable(param, "entry", false /* ignoreAssumes */);
			if (!addConstraintRet.Succeeded)
			{
				return addConstraintRet;
			}
		}

		//assumes of the return type can be ignored here (it will be used at call sites of this functions) 
		//as there is nothing to check and we can't use this information to help verification of this function
		ResultType addConstraintRet = addConstraintsForVariable(refinementData.Return, "entry", true /* ignoreAssumes */);
		if (!addConstraintRet.Succeeded)
		{
			return addConstraintRet;
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::BuildConstraintsFromInstructions(const RefinementMetadata& refinementData)
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
				else
				{
					return ResultType::Error("Unknown instruction type "s + instr.getOpcodeName());
				}
			}
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::ToString(std::string& output)
	{
		return constraintBuilder.ToStringOrFailure(output);
	}
}