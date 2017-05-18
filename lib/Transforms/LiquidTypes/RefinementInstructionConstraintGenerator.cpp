#include "llvm/Transforms/LiquidTypes/RefinementInstructionConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureInfo.h"
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
		else if (auto loadInstVal = dyn_cast<llvm::LoadInst>(&value))
		{
			binderName = loadInstructionNames[loadInstVal];
			return ResultType::Success();
		}
		else if (auto constantVal = dyn_cast<llvm::ConstantInt>(&value))
		{
			std::string constValue = constantVal->getValue().toString(10 /* radix */, false /* unsigned */);
			std::string possibleBinderName = "__constInt_" + constValue;

			if (!variableEnv.IsVariableDefined(possibleBinderName))
			{
				FixpointType fixpointType;
				auto convertResult = fixpointTypeConvertor.GetFixpointType(*(constantVal->getType()), fixpointType);
				if (!convertResult.Succeeded) { return convertResult; }

				std::string constraintString = "__value == "s + constValue;

				auto createBinderRes = variableEnv.CreateImmutableInputVariable(possibleBinderName, fixpointType, { constraintString });
				if (!createBinderRes.Succeeded) { return createBinderRes; }

				ResultType addQualRes = variableEnv.constraintBuilder.AddQualifierIfNew(possibleBinderName, { fixpointType }, { "__value" }, constraintString);
				if (!addQualRes.Succeeded) { return addQualRes; }
			}

			binderName = possibleBinderName;
			return ResultType::Success();
		}

		return ResultType::Error("Refinement Types: unknown constant type");
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

			left = variableEnv.GetVariableName(left);
			right = variableEnv.GetVariableName(right);
		}

		auto opRegisterName = binaryOpInst.getName().str();

		FixpointType fixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*(binaryOpInst.getType()), fixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		llvm::Type* retType = binaryOpInst.getType();
		if (retType->isIntegerTy())
		{
			std::string arithmeticExpressionForType = GetIntegerOperatorExpression(binaryOpInst, left, operatorString, right);

			auto addConstraintRes = variableEnv.CreateImmutableVariable(opRegisterName, fixpointType, { }, arithmeticExpressionForType);
			if (!addConstraintRes.Succeeded) { return addConstraintRes; }
		}
		else
		{
			return ResultType::Error("RefinementTypes: Binary operators not supported for type - " );
		}

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureReturnInstructionConstraint(const std::string& blockName, const ReturnInst& returnInst)
	{
		auto retVal = returnInst.getReturnValue();
		std::string retValStr;
		{
			auto getBinderNameRes= getBinderName(*retVal, retValStr);
			if (!getBinderNameRes.Succeeded) { return getBinderNameRes; }

			retValStr = variableEnv.GetVariableName(retValStr);
		}

		std::string assignedExpression = " __value == " + retValStr;

		auto addConstraintRes = variableEnv.AssignMutableVariable("return", assignedExpression);
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

			left = variableEnv.GetVariableName(left);

			auto rightRes = getBinderName(*(cmpInst.getOperand(1)), right);
			if (!rightRes.Succeeded) { return rightRes; }

			right = variableEnv.GetVariableName(right);
		}

		auto opRegisterName = cmpInst.getName().str();

		auto intType = dyn_cast<IntegerType>(cmpInst.getOperand(0)->getType());
		auto signedMaxIntVal = APInt::getSignedMaxValue(intType->getBitWidth());
		std::string signedMaxIntValStr = signedMaxIntVal.toString(10 /* radix */, false /* unsigned */);

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

		{
			auto addConstraintRes = variableEnv.CreateImmutableVariable(opRegisterName, FixpointType::GetBoolType(), {}, compareExpression);
			if (!addConstraintRes.Succeeded) { return addConstraintRes; }
		}

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

			left = variableEnv.GetVariableName(left);
		}

		FixpointType fixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*(zextInst.getType()), fixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		auto opRegisterName = zextInst.getName().str();

		std::string assignedExpression = convertFromBooleanType?
			(	"if (" + left + ") then __value == 1 else __value == 0") 
			:
			("__value == " + left);

		{
			auto addConstraintRes = variableEnv.CreateImmutableVariable(opRegisterName, fixpointType, {}, assignedExpression);
			if (!addConstraintRes.Succeeded) { return addConstraintRes; }
		}

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
			auto binderInfoRes = variableEnv.AddBranchInformation(conditionVar, true, conditionSuccessBlock, conditionFailBlock);
			if (!binderInfoRes.Succeeded) { return binderInfoRes; }
		}
		{
			auto binderInfoRes = variableEnv.AddBranchInformation(conditionVar, false , conditionFailBlock, conditionSuccessBlock);
			if (!binderInfoRes.Succeeded) { return binderInfoRes; }
		}

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CapturePhiInstructionConstraint(const std::string& blockName, const PHINode& phiInst)
	{
		auto operands = phiInst.operands();
		auto phiTargetVarName = phiInst.getName().str();
		auto targetLLVMType = phiInst.getType();

		FixpointType targetFixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*targetLLVMType, targetFixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		std::vector<std::string> phiVariables;
		std::vector<std::string> phiBlockNames;

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

			phiVariables.push_back(variableName);
			phiBlockNames.push_back(incomingBlockName);
		}

		auto createBinderRes = variableEnv.CreatePhiNode(phiTargetVarName, targetFixpointType, phiVariables, phiBlockNames);
		if (!createBinderRes.Succeeded) { return createBinderRes; }

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureSelectInstructionConstraint(const std::string& blockName, const SelectInst& selectInst)
	{
		auto selTargetVarName = selectInst.getName().str();
		auto targetLLVMType = selectInst.getType();

		FixpointType targetFixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*targetLLVMType, targetFixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		std::string conditionVar;
		{
			auto val = selectInst.getCondition();

			auto variableNameRes = getBinderName(*val, conditionVar);
			if (!variableNameRes.Succeeded) { return variableNameRes; }

			conditionVar = variableEnv.GetVariableName(conditionVar);
		}

		std::string trueVar;
		{
			auto val = selectInst.getTrueValue();

			auto variableNameRes = getBinderName(*val, conditionVar);
			if (!variableNameRes.Succeeded) { return variableNameRes; }

			trueVar = variableEnv.GetVariableName(trueVar);
		}

		std::string falseVar;
		{
			auto val = selectInst.getFalseValue();

			auto variableNameRes = getBinderName(*val, conditionVar);
			if (!variableNameRes.Succeeded) { return variableNameRes; }

			falseVar = variableEnv.GetVariableName(falseVar);
		}

		std::string assignedExpression = "if ("s + conditionVar + ") then __value == "s + trueVar + " else __value == "s + falseVar;
		{
			auto createBinderRes = variableEnv.CreateImmutableVariable(selTargetVarName, targetFixpointType, {}, assignedExpression);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

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
	ResultType RefinementInstructionConstraintGenerator::CaptureCallInstructionConstraint(const std::string& blockName, const CallInst& callInst, const std::string& callVariablesPrefixUsed, const RefinementFunctionSignatureInfo* callRefFunctionInfo)
	{
		//Add parameter constraints
		for (unsigned int i = 0, size = callRefFunctionInfo->ParsedFnRefinementMetadata.Parameters.size(); i < size; i++)
		{
			auto formalParam = callRefFunctionInfo->ParsedFnRefinementMetadata.Parameters[i];
			auto actualParam = callInst.getOperand(i);
			auto formalRegisterName = formalParam.LLVMName;
			auto actualRegisterName = actualParam->getName().str();

			{
				std::string variableTargetName = callVariablesPrefixUsed + formalRegisterName;

				auto addConstraintRes = variableEnv.AssignMutableVariable(variableTargetName, "__value == "s + actualRegisterName);
				if (!addConstraintRes.Succeeded) { return addConstraintRes; }
			}
		}

		//add return constraints
		{
			auto formalRegisterName = callInst.getName().str();

			FixpointType fixpointType;
			{
				auto llvmType = callRefFunctionInfo->ParsedFnRefinementMetadata.Return.LLVMType;
				auto convertResult = fixpointTypeConvertor.GetFixpointType(*llvmType, fixpointType);
				if (!convertResult.Succeeded) { return convertResult; }
			}

			std::string variableTargetName = callVariablesPrefixUsed + callRefFunctionInfo->ParsedFnRefinementMetadata.Return.LLVMName;

			{
				auto createBinderRes = variableEnv.CreateImmutableVariable(formalRegisterName, fixpointType, {}, "__value == "s + variableTargetName);
				if (!createBinderRes.Succeeded) { return createBinderRes; }
			}
		}

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureAllocaInstructionConstraint(const std::string& blockName, const AllocaInst& allocaInst)
	{
		std::string allocName = allocaInst.getName().str();
		auto targetPtrLLVMType = allocaInst.getType();

		if (!targetPtrLLVMType->isPointerTy())
		{
			return ResultType::Error("Expected pointer type in alloc");
		}

		auto targetLLVMType = targetPtrLLVMType->getPointerElementType();

		FixpointType targetFixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*targetLLVMType, targetFixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		{
			auto createBinderRes = variableEnv.CreateMutableVariable(allocName, targetFixpointType, {}, "true");
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureStoreInstructionConstraint(const std::string& blockName, const StoreInst& storeInst)
	{
		std::string storeSourceName;
		{
			auto variableNameRes = getBinderName(*storeInst.getValueOperand(), storeSourceName);
			if (!variableNameRes.Succeeded) { return variableNameRes; }
		}

		std::string storeTargetName = storeInst.getPointerOperand()->getName().str();

		{
			std::string assignedExpr = "__value == "s + variableEnv.GetVariableName(storeSourceName);

			auto createBinderRes = variableEnv.AssignMutableVariable(storeTargetName, assignedExpr);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureLoadInstructionConstraint(const std::string& blockName, const LoadInst& loadInst)
	{
		std::string regName = loadInst.getName().str();
		if (regName == "")
		{
			regName = "__load" + variableEnv.FreshState.GetNextIdString();
			loadInstructionNames[&loadInst] = regName;
		}
		
		std::string loadSourceName = loadInst.getPointerOperand()->getName().str();

		auto targetLLVMType = loadInst.getType();
		FixpointType targetFixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*targetLLVMType, targetFixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		{
			std::string assignedExpr = "__value == "s + variableEnv.GetVariableName(loadSourceName);

			auto createBinderRes = variableEnv.CreateImmutableVariable(regName, targetFixpointType, {}, assignedExpr);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}
		return ResultType::Success();
	}

	ResultType RefinementInstructionConstraintGenerator::CaptureBlockConstraints(const std::string& blockName)
	{
		return variableEnv.StartBlock(blockName);
	}
}