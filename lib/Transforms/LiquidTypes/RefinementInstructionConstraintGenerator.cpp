#include "llvm/Transforms/LiquidTypes/RefinementInstructionConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionInfo.h"
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
				std::string constraintString = "__value == " + constValue;
				variableConstraints.push_back(constraintString);

				FixpointBaseType fixpointType;
				auto convertResult = fixpointTypeConvertor.GetFixpointType(*(constantVal->getType()), fixpointType);
				if (!convertResult.Succeeded) { return convertResult; }

				auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(possibleBinderName, fixpointType, variableConstraints);
				if (!createBinderRes.Succeeded) { return createBinderRes; }

				ResultType addQualRes = constraintBuilder.AddQualifierIfNew(possibleBinderName, { fixpointType }, { "__value" }, constraintString);
				if (!addQualRes.Succeeded) { return addQualRes; }
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

	ResultType RefinementInstructionConstraintGenerator::CaptureComparisonInstructionConstraint(const std::string& blockName, const CmpInst& cmpInst)
	{
		std::string operatorString = "";
		std::string operatorDesc = "";
		bool signedOp = false;

		auto cmpOperator = cmpInst.getPredicate();

		switch (cmpOperator)
		{
			case CmpInst::ICMP_SLT:
				signedOp = true;
			case CmpInst::ICMP_ULT:
				operatorString = "<";
				operatorDesc = "_cmplessthan";
				break;
			case CmpInst::ICMP_SGT:
				signedOp = true;
			case CmpInst::ICMP_UGT:
				operatorString = ">";
				operatorDesc = "_cmpgreaterthan";
				break;
			case CmpInst::ICMP_SLE:
				signedOp = true;
			case CmpInst::ICMP_ULE:
				operatorString = "<=";
				operatorDesc = "_cmplessthaneq";
				break;
			case CmpInst::ICMP_SGE:
				signedOp = true;
			case CmpInst::ICMP_UGE:
				operatorString = ">=";
				operatorDesc = "_cmpgreaterthaneq";
				break;
			case CmpInst::ICMP_EQ:
				operatorString = "==";
				operatorDesc = "_compareeq";
				break;
			case CmpInst::ICMP_NE:
				operatorString = "!=";
				operatorDesc = "_comparenoteq";
				break;
			default:
				return ResultType::Error("Refinement Types: Unknown comparison type - " + cmpOperator);
		}

		std::string left, right;
		{
			auto leftRes = getBinderName(*(cmpInst.getOperand(0)), left);
			if (!leftRes.Succeeded) { return leftRes; }

			auto rightRes = getBinderName(*(cmpInst.getOperand(1)), right);
			if (!rightRes.Succeeded) { return rightRes; }
		}

		auto variablesInScope = variableEnv.GetVariablesInScope(blockName);
		auto variablesInfo = variableEnv.GetVariablesInfo(blockName);

		auto opRegisterName = cmpInst.getName().str();
		variableEnv.AddVariable(blockName, opRegisterName);
		auto createBinderRes = constraintBuilder.CreateBinder(opRegisterName, liquid::FixpointBaseType::BOOL, variablesInScope, variablesInfo);
		if (!createBinderRes.Succeeded) { return createBinderRes; }

		auto intType = dyn_cast<IntegerType>(cmpInst.getOperand(0)->getType());
		auto signedMaxIntVal = APInt::getSignedMaxValue(intType->getBitWidth());
		std::string signedMaxIntValStr = signedMaxIntVal.toString(10 /* radix */, false /* unsigned */);

		std::string constraintName = opRegisterName + operatorDesc;
		std::string compareExpression;

		if (signedOp)
		{
			//std::string signedMax, signedMaxIntValStr;
			std::string leftWrap  = "("s + left  + " - 1 - "s + signedMaxIntValStr + ")"s;
			std::string rightWrap = "("s + right + " - 1 - "s + signedMaxIntValStr + ")"s;

			compareExpression =
				"if ("s + left + " > "s + signedMaxIntValStr + " && " + right + " > "s + signedMaxIntValStr + ") then "s +
				"__value <=> ("s + leftWrap + operatorString + rightWrap + " ) "s +

				"elseif ("s + left + " > "s + signedMaxIntValStr + ") then "s +
				"__value <=> ("s + leftWrap + operatorString + right + " ) "s +

				"elseif ("s + right + " > "s + signedMaxIntValStr + ") then "s +
				"__value <=> ("s + left + operatorString + rightWrap + " ) "s +

				"else " +
				"__value <=> ("s + left + operatorString + right + " )"s;
		}
		else
		{
			compareExpression = "__value <=> ("s + left + operatorString + right + " )"s;
		}

		auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, opRegisterName, compareExpression, variablesInScope, {} /* future binders needed only for phi nodes */, variablesInfo);
		if (!addConstraintRes.Succeeded) { return addConstraintRes; }

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureZeroExtendInstructionConstraint(const std::string& blockName, const ZExtInst& zextInst)
	{
		bool convertFromBooleanType = false;
		auto leftOperand = zextInst.getOperand(0);
		{
			if (auto intType = dyn_cast<IntegerType>(leftOperand->getType()))
			{
				if (intType->getBitWidth() == 1) { convertFromBooleanType = true; }
			}
		}

		std::string left;
		{
			auto leftRes = getBinderName(*leftOperand, left);
			if (!leftRes.Succeeded) { return leftRes; }
		}

		FixpointBaseType fixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*(zextInst.getType()), fixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		auto variablesInScope = variableEnv.GetVariablesInScope(blockName);
		auto variablesInfo = variableEnv.GetVariablesInfo(blockName);
		auto opRegisterName = zextInst.getName().str();
		variableEnv.AddVariable(blockName, opRegisterName);

		auto createBinderRes = constraintBuilder.CreateBinder(opRegisterName, fixpointType, variablesInScope, variablesInfo);
		if (!createBinderRes.Succeeded) { return createBinderRes; }

		std::string assignedExpression;
		if (convertFromBooleanType)
		{
			assignedExpression ="if (" + left + ") then __value == 1 else __value == 0";
		}
		else
		{
			assignedExpression = "__value == " + left;
		}

		auto freshNameSuffix = constraintBuilder.GetFreshNameSuffix();
		auto constraintName = "zext_" + std::to_string(freshNameSuffix);
		auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, opRegisterName, assignedExpression, variablesInScope, {} /* future binders needed only for phi nodes */, variablesInfo);
		if (!addConstraintRes.Succeeded) { return addConstraintRes; }

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureBranchInstructionConstraint(const std::string& blockName, const BranchInst& brInst)
	{
		if (brInst.isUnconditional())
		{
			//don't need to do anything here
			return ResultType::Success();
		}

		auto conditionVarValue = brInst.getCondition();
		std::string conditionVar;
		{
			auto conditionVarRes = getBinderName(*conditionVarValue, conditionVar);
			if (!conditionVarRes.Succeeded) { return conditionVarRes; }
		}

		auto conditionSuccessBlock = brInst.getSuccessor(0)->getName().str();
		auto conditionFailBlock = brInst.getSuccessor(1)->getName().str();

		{
			auto infoNameTrue = conditionVar + "_" + blockName + "_" + "branch_true";
			variableEnv.AddVariableInfo(conditionSuccessBlock, infoNameTrue);
			std::vector<std::string> binderInfoQualifiers;
			binderInfoQualifiers.push_back("__value");
			ResultType binderInfoRes = constraintBuilder.AddBinderInformation(infoNameTrue, conditionVar, binderInfoQualifiers);
			if (!binderInfoRes.Succeeded) { return binderInfoRes; }
		}
		{
			auto infoNameFalse = conditionVar + "_" + blockName + "_" + "branch_false";
			variableEnv.AddVariableInfo(conditionFailBlock, infoNameFalse);
			std::vector<std::string> binderInfoQualifiers;
			binderInfoQualifiers.push_back("~__value");
			ResultType binderInfoRes = constraintBuilder.AddBinderInformation(infoNameFalse, conditionVar, binderInfoQualifiers);
			if (!binderInfoRes.Succeeded) { return binderInfoRes; }
		}

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CapturePhiInstructionConstraint(const std::string& blockName, const PHINode& phiInst)
	{
		auto operands = phiInst.operands();
		auto phiTargetVarName = phiInst.getName().str();
		auto targetLLVMType = phiInst.getType();

		FixpointBaseType targetFixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*targetLLVMType, targetFixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		{
			auto variablesInScope = variableEnv.GetVariablesInScope(blockName);
			auto variablesInfo = variableEnv.GetVariablesInfo(blockName);
			auto createBinderRes = constraintBuilder.CreateBinder(phiTargetVarName, targetFixpointType, variablesInScope, variablesInfo);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		for (auto& operand : operands)
		{
			auto val = operand.get();
			std::string variableName;
			{
				auto variableNameRes = getBinderName(*val, variableName);
				if (!variableNameRes.Succeeded) { return variableNameRes; }
			}

			auto incomingBlock = phiInst.getIncomingBlock(operand);
			auto incomingBlockName = incomingBlock->getName().str();
			auto incomingVariablesInScope = variableEnv.GetVariablesInScope(incomingBlockName);
			auto incomingVariablesInfo = variableEnv.GetVariablesInfo(incomingBlockName);

			std::vector<std::string> futureVariables;

			if (isLLVMRegister(*val))
			{
				if (std::find(incomingVariablesInScope.begin(), incomingVariablesInScope.end(), variableName) == incomingVariablesInScope.end())
				{
					//if this is a variable, which is not in scope, this may not have been created yet
					//consider the following example
					//
					//entry:
					//  br for.cond  
					//for.cond
					//  i.0 = phi 0  inc
					//	...
					//  br for.body
					//for.body
					//  inc = add ret.0  1  
					//  br for.cond  
					//  ...
					//
					//Here inc is used before its actual definition while reading top down
					//we need to guard against this
					auto futureBinderRes = constraintBuilder.CreateFutureBinder(variableName, targetFixpointType);
					if (!futureBinderRes.Succeeded) { return futureBinderRes; }
					futureVariables.push_back(variableName);
				}
			}

			auto assignmentQualifier = "__value == " + variableName;

			auto constraintName = blockName + "_phi_" + variableName;
			auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, phiTargetVarName, assignmentQualifier, incomingVariablesInScope, futureVariables, incomingVariablesInfo);
			if (!addConstraintRes.Succeeded) { return addConstraintRes; }
		}

		variableEnv.AddVariable(blockName, phiTargetVarName);

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureSelectInstructionConstraint(const std::string& blockName, const SelectInst& selectInst)
	{
		auto selTargetVarName = selectInst.getName().str();
		auto targetLLVMType = selectInst.getType();

		FixpointBaseType targetFixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*targetLLVMType, targetFixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		auto variablesInScope = variableEnv.GetVariablesInScope(blockName);

		{
			auto variablesInfo = variableEnv.GetVariablesInfo(blockName);
			auto createBinderRes = constraintBuilder.CreateBinder(selTargetVarName, targetFixpointType, variablesInScope, variablesInfo);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		for (int i = 0; i < 2; i++)
		{
			auto operandVal = selectInst.getTrueValue();
			auto binderInfoForThisOperand = "__value";

			if (i == 1)
			{
				operandVal = selectInst.getFalseValue();
				binderInfoForThisOperand = "~__value";
			}

			std::string variableName;
			{
				auto variableNameRes = getBinderName(*operandVal, variableName);
				if (!variableNameRes.Succeeded) { return variableNameRes; }
			}
			auto assignmentQualifier = "__value == " + variableName;

			auto incomingVariablesInfo = variableEnv.GetVariablesInfo(blockName);
			incomingVariablesInfo.push_back(binderInfoForThisOperand);

			auto constraintName = blockName + "_select_" + variableName;
			auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, selTargetVarName, assignmentQualifier, variablesInScope, {} /* future binders needed only for phi nodes */, incomingVariablesInfo);
			if (!addConstraintRes.Succeeded) { return addConstraintRes; }
		}

		variableEnv.AddVariable(blockName, selTargetVarName);

		return ResultType::Success();
	}

	namespace {
		std::vector<std::string> getNonDependentConstraints(std::string variableConstraint)
		{
			//TODO:
			std::vector<std::string> ret;
			if (variableConstraint != "")
			{
				ret.push_back(variableConstraint);
			}

			return ret;
		}
	}

	//At the time this function is called, it is expected that variables exist representing the formal arguments and return of the callee
	//For example, if the prefixString is "pre_", for a call to function
	//int {assumeStr, verifyStr} bar(int{assumeStr, verifyStr} a);
	//
	//int y = bar(x);
	//
	//We expect the variables 
	//pre_ret{assumeStr, verifyStr}, pre_a{verifyStr} to already be generated
	//
	//Thus all we have to do is create assignments from
	//x to pre_a
	//and pre_ret to y
	ResultType RefinementInstructionConstraintGenerator::CaptureCallInstructionConstraint(const std::string& blockName, const CallInst& callInst, const std::string& callVariablesPrefixUsed, const RefinementFunctionInfo* callRefFunctionInfo)
	{
		auto variablesInScope = variableEnv.GetVariablesInScope(blockName);
		auto variablesInfo = variableEnv.GetVariablesInfo(blockName);

		//Add parameter constraints
		for (unsigned int i = 0, size = callRefFunctionInfo->ParsedFnRefinementMetadata.Parameters.size(); i < size; i++)
		{
			auto formalParam = callRefFunctionInfo->ParsedFnRefinementMetadata.Parameters[i];
			auto actualParam = callInst.getOperand(i);
			auto formalRegisterName = formalParam.LLVMName;
			auto actualRegisterName = actualParam->getName().str();

			{
				std::string constraintName = actualRegisterName + "_param_"s + formalRegisterName;
				std::string variableTargetName = callVariablesPrefixUsed + formalRegisterName;
				std::string argumentAssignment = "__value == " + actualRegisterName;
				auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, variableTargetName, argumentAssignment, variablesInScope, {} /* future binders needed only for phi nodes */, variablesInfo);
				if (!addConstraintRes.Succeeded) { return addConstraintRes; }
			}
		}

		//add return constraints
		{
			auto formalRegisterName = callInst.getName().str();

			FixpointBaseType fixpointType;
			{
				auto llvmType = callRefFunctionInfo->ParsedFnRefinementMetadata.Return.LLVMType;
				auto convertResult = fixpointTypeConvertor.GetFixpointType(*llvmType, fixpointType);
				if (!convertResult.Succeeded) { return convertResult; }
			}

			{
				auto createBinderRes = constraintBuilder.CreateBinder(formalRegisterName, fixpointType, variablesInScope, variablesInfo);
				if (!createBinderRes.Succeeded) { return createBinderRes; }
			}

			{
				std::string constraintName = formalRegisterName + "_param_"s + callRefFunctionInfo->ParsedFnRefinementMetadata.Return.LLVMName;
				std::string variableTargetName = callVariablesPrefixUsed + callRefFunctionInfo->ParsedFnRefinementMetadata.Return.LLVMName;
				std::string argumentAssignment = "__value == " + variableTargetName;
				auto addConstraintRes = constraintBuilder.AddConstraintForAssignment(constraintName, formalRegisterName, argumentAssignment, variablesInScope, {} /* future binders needed only for phi nodes */, variablesInfo);
				if (!addConstraintRes.Succeeded) { return addConstraintRes; }
			}

			variableEnv.AddVariable(blockName, formalRegisterName);
		}

		return ResultType::Success();
	}

}