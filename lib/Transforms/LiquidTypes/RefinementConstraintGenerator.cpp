#include "llvm/Transforms/LiquidTypes/RefinementConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"

#include <map>

using namespace std::literals::string_literals;

namespace liquid {

	std::string RefinementConstraintGenerator::getMaxValueForIntWidth(int width)
	{
		std::string maxVal = APInt::getMaxValue(width).toString(10 /* radix */, false /* unsigned */);
		return maxVal;
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
			auto addQualRes = variableEnv.constraintBuilder.AddQualifierIfNew(constraintName + std::to_string(i), { FixpointType::GetIntType() }, { "__value" }, constraint);
			if (!addQualRes.Succeeded) { return addQualRes; }
			i++;
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::addConstraintsForVariable(
		const RefinementMetadataForVariable& variable, 
		const std::string& prefix, 
		const std::string& blockName, 
		const bool ignoreAssumes,
		const bool isReturnType,
		const std::map<std::string, std::string>& variableReplacements,
		const std::map<std::string, FixpointType>& variableTypes
	)
	{
		std::vector<std::string> variableConstraints;

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

		std::vector<RefinementPiece> refinementPiecesForNewQualifiers;

		if (!ignoreAssumes)
		{
			if (variable.Assume.OriginalRefinementString != "")
			{
				refinementPiecesForNewQualifiers = RefinementUtils::VectorAppend(refinementPiecesForNewQualifiers, variable.Assume.GetRefinementParts());

				ParsedRefinement refinementToUse = variable.Assume;
				if (!variableReplacements.empty())
				{
					refinementToUse = variable.Assume.ReplaceVariables(variableReplacements);
				}

				std::vector<RefinementPiece> refinementPieces = refinementToUse.GetRefinementParts();
				std::vector<std::string> refinementPiecesStr = RefinementUtils::SelectString(refinementPieces, [](auto &refPiece) { return refPiece.RefinementPieceText; });
				variableConstraints = RefinementUtils::VectorAppend(variableConstraints, refinementPiecesStr);
			}
		}

		if (variable.Verify.OriginalRefinementString != "")
		{
			refinementPiecesForNewQualifiers = RefinementUtils::VectorAppend(refinementPiecesForNewQualifiers, variable.Verify.GetRefinementParts());

			ParsedRefinement refinementToUse = variable.Verify;
			if (!variableReplacements.empty())
			{
				refinementToUse = variable.Verify.ReplaceVariables(variableReplacements);
			}

			std::vector<RefinementPiece> refinementPieces = refinementToUse.GetRefinementParts();
			std::vector<std::string> refinementPiecesStr = RefinementUtils::SelectString(refinementPieces, [](auto &refPiece) { return refPiece.RefinementPieceText; });
			variableConstraints = RefinementUtils::VectorAppend(variableConstraints, refinementPiecesStr);
		}

		FixpointType fixpointType;
		{
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*(variable.LLVMType), fixpointType);
			if (!convertResult.Succeeded) { return convertResult; }
		}

		std::string currVarName = prefix + variable.LLVMName;
		{

			auto createBinderRes = 
				isReturnType? variableEnv.CreateMutableOutputVariable(currVarName, fixpointType, variableConstraints)
				: variableEnv.CreateImmutableInputVariable(currVarName, fixpointType, variableConstraints);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		for (const auto& refinementPiecesForNewQualifier : refinementPiecesForNewQualifiers)
		{
			std::vector<std::string> vars(refinementPiecesForNewQualifier.VariablesUsed.begin(), refinementPiecesForNewQualifier.VariablesUsed.end());
			std::vector<FixpointType> varTypes;
			for (auto& var : vars)
			{
				if(!RefinementUtils::ContainsKey(variableTypes, var))
				{
					return ResultType::Error("Refinement Types: Could not find type for variable " + var);
				}

				varTypes.push_back(variableTypes.at(var));
			}

			vars.push_back("__value");
			varTypes.push_back(fixpointType);

			ResultType addQualRes = variableEnv.constraintBuilder.AddQualifierIfNew(currVarName + variableEnv.FreshState.GetNextIdString(), varTypes, vars, refinementPiecesForNewQualifier.RefinementPieceText);
			if (!addQualRes.Succeeded) { return addQualRes; }
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::buildConstraintsFromSignatureForBlock(
		const RefinementMetadata& refinementData, 
		const std::string& prefix, 
		const std::string& blockName, 
		const bool ignoreParameterAssumes, 
		const bool ignoreReturnAssumes
	)
	{
		std::map<std::string, std::string> variableReplacements;
		std::map<std::string, FixpointType> variableTypes;

		std::vector<RefinementMetadataForVariable> paramsAndRet = RefinementUtils::VectorAppend(refinementData.Parameters, { refinementData.Return });
		//Find variable renamings
		for (auto& param : paramsAndRet)
		{
			const std::string originalVarName = param.OriginalName;
			const std::string llvmVarName = prefix + param.LLVMName;

			if (originalVarName != llvmVarName)
			{
				variableReplacements[originalVarName] = llvmVarName;
			}

			FixpointType fixpointType;
			auto convertResult = fixpointTypeConvertor.GetFixpointType(*(param.LLVMType), fixpointType);
			if (!convertResult.Succeeded) { return convertResult; }

			variableTypes[originalVarName] = fixpointType;
		}

		for (auto& param : refinementData.Parameters)
		{
			ResultType addConstraintRet = addConstraintsForVariable(param, prefix, blockName, ignoreParameterAssumes /* ignoreAssumes */, false /* isReturnType*/, variableReplacements, variableTypes);
			if (!addConstraintRet.Succeeded)
			{
				return addConstraintRet;
			}
		}

		//assumes of the return type can be ignored here (it will be used at call sites of this functions) 
		//as there is nothing to check and we can't use this information to help verification of this function
		ResultType addConstraintRet = addConstraintsForVariable(refinementData.Return, prefix, blockName, ignoreReturnAssumes /* ignoreAssumes */, true /* isReturnType*/, variableReplacements, variableTypes);
		if (!addConstraintRet.Succeeded)
		{
			return addConstraintRet;
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::BuildConstraintsFromSignature(const RefinementMetadata& refinementData)
	{
		auto startBlockRes = variableEnv.StartBlock("entry");
		if (!startBlockRes.Succeeded) { return startBlockRes; }

		return buildConstraintsFromSignatureForBlock(refinementData, "" /* prefix */, "entry", false /* ignoreParameterAssumes */, true /* ignoreReturnAssumes */);
	}

	ResultType RefinementConstraintGenerator::generateCallSignatureVariables(const std::string& blockName, const CallInst& callInst, const AnalysisRetriever& analysisRetriever, std::string& prefixUsed, const RefinementFunctionSignatureInfo* &callFunctionInfo)
	{
		auto callFn = callInst.getCalledFunction();

		if (analysisRetriever.ContainsAnalysisForFunction(*callFn))
		{
			callFunctionInfo = analysisRetriever.GetAnalysisForFunction(*callFn);

			auto opRegisterName = callInst.getName().str();
			prefixUsed = opRegisterName + "_";
			return buildConstraintsFromSignatureForBlock(callFunctionInfo->ParsedFnRefinementMetadata, prefixUsed, blockName, true /* ignoreParameterAssumes */, false /* ignoreReturnAssumes */);
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::BuildConstraintsFromInstructions(const RefinementMetadata& refinementData, llvm::AAResults& aliasAnalysis, const AnalysisRetriever& analysisRetriever)
	{
		bool first = true;
		for (auto& block : Func)
		{
			auto blockName = block.getName().str();
			{
				//the first block has already been started in the function signature creation
				if (first)
				{
					first = false;
				}
				else
				{
					ResultType res = instructionConstraintBuilder.CaptureBlockConstraints(blockName);
					if (!res.Succeeded) { return res; }
				}
			}

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
					const RefinementFunctionSignatureInfo* callRefFunctionInfo;
					{
						ResultType res = generateCallSignatureVariables(blockName, *callInst, analysisRetriever, prefixUsed, callRefFunctionInfo);
						if (!res.Succeeded) { return res; }
					}

					ResultType res = instructionConstraintBuilder.CaptureCallInstructionConstraint(blockName, *callInst, prefixUsed, callRefFunctionInfo);
					if (!res.Succeeded) { return res; }
				}
				else if (auto allocaInst = dyn_cast<AllocaInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureAllocaInstructionConstraint(blockName, *allocaInst);
					if (!res.Succeeded) { return res; }
				}
				else if (auto storeInst = dyn_cast<StoreInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureStoreInstructionConstraint(blockName, *storeInst, aliasAnalysis);
					if (!res.Succeeded) { return res; }
				}
				else if (auto loadInst = dyn_cast<LoadInst>(&instr))
				{
					ResultType res = instructionConstraintBuilder.CaptureLoadInstructionConstraint(blockName, *loadInst, aliasAnalysis);
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
				ResultType addQualRes = variableEnv.constraintBuilder.AddQualifierIfNew("loopStepQualifier", { FixpointType::GetIntType(), FixpointType::GetIntType(), FixpointType::GetIntType() }, { "v", "a", "b" }, "(v - a) mod b == 0");
				if (!addQualRes.Succeeded) { return addQualRes; }
			}
		}

		return ResultType::Success();
	}

	ResultType RefinementConstraintGenerator::ToString(std::string& output)
	{
		return variableEnv.ToStringOrFailure(output);
	}
}