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

	std::vector<std::string> RefinementConstraintGenerator::registerAndRetrieveIntegerQualifiers(const llvm::IntegerType& type)
	{
		std::string maxIntVal = getMaxValueForIntWidth(type.getIntegerBitWidth());
		std::vector<std::string> constraints { "__value >= 0"s, "__value <= "s + maxIntVal };

		std::string constraintName = "Int" + std::to_string(type.getBitWidth()) + "Limit";

		unsigned int i = 0;
		for (auto& constraint : constraints)
		{
			constraintBuilder.AddQualifierIfNew(constraintName + std::to_string(i), { FixpointBaseType::INT }, { "__value" }, constraint);
			i++;
		}

		return constraints;
	}

	ResultType RefinementConstraintGenerator::addConstraintsForVariable(const RefinementMetadataForVariable& variable, const std::string& blockName, bool ignoreAssumes)
	{
		std::vector<std::string> variableConstraints;
		FixpointBaseType fixpointType;

		if (variable.LLVMType->isIntegerTy())
		{
			auto intType = dyn_cast<llvm::IntegerType>(variable.LLVMType);
			variableConstraints = registerAndRetrieveIntegerQualifiers(*intType);
			fixpointType = (intType->getIntegerBitWidth() == 1)? FixpointBaseType::BOOL : FixpointBaseType::INT;
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
		constraintBuilder.CreateBinderWithQualifiers(variable.LLVMName, fixpointType, variableConstraints);
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

		//assumes of the return type can be ignored as there is nothing to check and we can't use this information 
		//to help verification of this function
		ResultType addConstraintRet = addConstraintsForVariable(refinementData.Return, "entry", true /* ignoreAssumes */);
		if (!addConstraintRet.Succeeded)
		{
			return addConstraintRet;
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::ToString(std::string& output)
	{
		bool success = constraintBuilder.ToStringOrFailure(output);
		if (!success)
		{
			return ResultType::Error("Refinement types: constraint generator failed");
		}
		return ResultType::Success();
	}
}