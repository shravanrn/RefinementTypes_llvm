#include "llvm/Transforms/LiquidTypes/RefinementInstructionConstraintGenerator.h"
#include "llvm/IR/Constants.h"

using namespace std::literals::string_literals;

namespace liquid {

	bool RefinementInstructionConstraintGenerator::isLLVMRegister(const llvm::Value& value)
	{
		std::string str = value.getName().str();
		auto ret = (str != "");
		return ret;
	}

	ResultType RefinementInstructionConstraintGenerator::getBinderName(const llvm::Value& value, std::string& binderName)
	{
		std::string ret = "";
		if (isLLVMRegister(value))
		{
			binderName = value.getName().str();
			return ResultType::Success();
		}
		else if (auto constantVal = dyn_cast<llvm::ConstantInt>(&value))
		{
			std::string constValue = constantVal->getValue().toString(10 /* radix */, false /* unsigned */);
			std::string possibleBinderName = "__constInt_" + constValue;

			if (!constraintBuilder.DoesBinderExist(possibleBinderName))
			{
				//this is a constant, so just add it to the top block. No point adding it to the current block
				variableEnv.AddVariable("entry", possibleBinderName);
				std::vector<std::string> variableConstraints;
				variableConstraints.push_back("__value == " + constValue);

				FixpointBaseType fixpointType;
				auto convertResult = fixpointTypeConvertor.GetFixpointType(*(constantVal->getType()), fixpointType);
				if (!convertResult.Succeeded) { return convertResult; }

				auto createBinderRes = constraintBuilder.CreateBinderWithQualifiers(possibleBinderName, fixpointType, variableConstraints);
				if (!createBinderRes.Succeeded) { return createBinderRes; }
			}

			binderName = possibleBinderName;
			return ResultType::Success();
		}

		return ResultType::Error("Refinement Types: unknown constant type" + value.getType()->getStructName().str());
	}

	std::string RefinementInstructionConstraintGenerator::getSignedIntFromUnsignedRepresentation(const std::string& unsignedValStr, unsigned int width)
	{
		// for width 4
		// if input is 0b1111 -> 15, we need -1
		// if input is 0b1110 -> 14, we need -2
		// this is unsignedValStr - 2 ^ n
		// for numbers which such as 0b0111 -> 7, we need 7

		std::string signedMaxVal = (APInt::getSignedMaxValue(width)).toString(10 /* radix */, true /* signed */);
		std::string unsignedMaxVal = (APInt::getMaxValue(width)).toString(10 /* radix */, false /* unsigned */);
		std::string convertedVal = unsignedValStr + " - " + unsignedMaxVal;

		std::string ret = "if ("s + unsignedValStr + " > "s + signedMaxVal + ") then ("s + convertedVal + ") else ("s + unsignedValStr + ")"s;
		return ret;
	}

	std::string RefinementInstructionConstraintGenerator::GetIntegerOperatorExpression(
		const BinaryOperator& binaryOpInst,
		const std::string& left,
		const std::string& operatorString,
		const std::string& right
	)
	{
		std::string arithmeticExpressionForType;

		std::string arithmeticExpression = left + operatorString + right;
		auto intType = dyn_cast<IntegerType>(binaryOpInst.getType());

		auto maxIntVal = APInt::getMaxValue(intType->getBitWidth());
		std::string maxIntValStr = maxIntVal.toString(10 /* radix */, false /* unsigned */);

		if (binaryOpInst.hasNoSignedWrap())
		{
			std::string signedMax = APInt::getSignedMaxValue(intType->getBitWidth()).toString(10 /* radix */, true /* signed */);
			std::string signedMin = APInt::getSignedMinValue(intType->getBitWidth()).toString(10 /* radix */, true /* signed */);
			std::string leftSigned = getSignedIntFromUnsignedRepresentation(left, intType->getBitWidth());
			std::string rightSigned = getSignedIntFromUnsignedRepresentation(right, intType->getBitWidth());
			std::string arithmeticSignedAddExpression = leftSigned + operatorString + rightSigned;

			// (signedMin <= (signed_left + signed_right) <= signedMax) <=> val = left + right
			arithmeticExpressionForType =
				"("s +
				"("s + arithmeticSignedAddExpression + ") >= "s + signedMin + " && "s +
				"("s + arithmeticSignedAddExpression + ") <= "s + signedMax +
				") "s +
				"<=> __value == ("s + arithmeticExpression + " )"s;
		}
		else
		{
			if (binaryOpInst.hasNoUnsignedWrap())
			{
				// ((left + right) <= unsignedMax) <=> val = left + right
				arithmeticExpressionForType =
					"("s + arithmeticExpression + ") "s + " <= "s + maxIntValStr +
					"<=> __value == ("s + arithmeticExpression + " )"s;
			}
			else
			{
				// ((left + right) <= unsignedMax) <=> val = left + right &&
				//		((left + right) > unsignedMax) <=> val = left + right - 1 - 2^n
				arithmeticExpressionForType =
					"if ("s + arithmeticExpression + " <= "s + maxIntValStr + ") then "s +
					"__value == ("s + arithmeticExpression + " ) "s +
					"else " +
					"__value == ("s + arithmeticExpression + " - 1 - "s + maxIntValStr + " )"s;
			}
		}

		return arithmeticExpressionForType;
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureBinaryOperatorConstraint(const std::string& blockName, const BinaryOperator& binaryOpInst)
	{
		std::string operatorString = "";
		std::string operatorDesc = "";

		if (binaryOpInst.getOpcode() == BinaryOperator::Add)
		{
			operatorString = "+";
			operatorDesc = "_add";
		}
		else if (binaryOpInst.getOpcode() == BinaryOperator::Sub)
		{
			operatorString = "-";
			operatorDesc = "_sub";
		}
		else if (binaryOpInst.getOpcode() == BinaryOperator::Mul)
		{
			operatorString = "*";
			operatorDesc = "_mult";
		}
		else
		{
			return ResultType::Error("Refinement types: Unknown binary operation"s + binaryOpInst.getOpcodeName());
		}

		std::string left, right;
		{
			auto leftRes = getBinderName(*(binaryOpInst.getOperand(0)), left);
			if (!leftRes.Succeeded) { return leftRes; }

			auto rightRes = getBinderName(*(binaryOpInst.getOperand(1)), right);
			if (!rightRes.Succeeded) { return rightRes; }
		}

		auto variablesInScope = variableEnv.GetVariablesInScope(blockName);
		auto variablesInfo = variableEnv.GetVariablesInfo(blockName);

		auto opRegisterName = binaryOpInst.getName().str();
		std::string constraintName = opRegisterName + operatorDesc;

		variableEnv.AddVariable(blockName, opRegisterName);

		FixpointBaseType fixpointType;
		auto convertResult = fixpointTypeConvertor.GetFixpointType(*(binaryOpInst.getType()), fixpointType);
		if (!convertResult.Succeeded) { return convertResult; }

		auto createBinderRes = constraintBuilder.CreateBinder(opRegisterName, fixpointType, variablesInScope, variablesInfo);
		if (!createBinderRes.Succeeded) { return createBinderRes; }

		llvm::Type* retType = binaryOpInst.getType();
		if (retType->isIntegerTy())
		{
			std::string arithmeticExpressionForType = GetIntegerOperatorExpression(binaryOpInst, left, operatorString, right);

			auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, 
				opRegisterName, 
				arithmeticExpressionForType, 
				variablesInScope, 
				{} /* future binders needed only for phi nodes */, variablesInfo);

			if (!addConstraintRes.Succeeded) { return addConstraintRes; }
		}
		else
		{
			return ResultType::Error("RefinementTypes: Binary operators not supported for type - " + retType->getStructName().str());
		}

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureReturnInstructionConstraint(const std::string& blockName, const ReturnInst& returnInst)
	{
		auto retVal = returnInst.getReturnValue();
		std::string retValStr;
		auto getBinderNameRes= getBinderName(*retVal, retValStr);
		if (!getBinderNameRes.Succeeded) { return getBinderNameRes; }

		auto variablesInScope = variableEnv.GetVariablesInScope(blockName);
		auto variablesInfo = variableEnv.GetVariablesInfo(blockName);

		std::string assignedExpression = " __value == " + retValStr;

		auto freshNameSuffix = constraintBuilder.GetFreshNameSuffix();
		auto constraintName = "return_" + std::to_string(freshNameSuffix);
		auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, "return", assignedExpression, variablesInScope, {} /* future binders needed only for phi nodes */, variablesInfo);
		if (!addConstraintRes.Succeeded) { return addConstraintRes; }

		return ResultType::Success();
	}

}