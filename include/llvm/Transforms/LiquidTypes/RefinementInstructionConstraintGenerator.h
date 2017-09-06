#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSINSTRUCTIONCONSTRAINTGENERATOR_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSINSTRUCTIONCONSTRAINTGENERATOR_H

#include <string>
#include <map>
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/FixpointTypeConvertor.h"
#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Analysis/AliasAnalysis.h"

using namespace llvm;

namespace liquid {
	
	class RefinementFunctionSignatureInfo;

	class RefinementInstructionConstraintGenerator
	{
	private:
		FixpointTypeConvertor& fixpointTypeConvertor;
		VariablesEnvironment& variableEnv;

		std::map<const llvm::Instruction*, std::string> loadInstructionNames;
		std::map<std::string, const llvm::AllocaInst*> allocaVariables;

		bool isLLVMRegister(const llvm::Value& value);
		ResultType getBinderName(const llvm::Value& value, std::string& binderName);
		std::string getSignedIntFromUnsignedRepresentation(const std::string& unsignedValStr, unsigned int width);

		std::string GetIntegerOperatorExpression(
			const BinaryOperator& binaryOpInst,
			const std::string& left,
			const std::string& operatorString,
			const std::string& right
		);

		std::string getValueVariable(const std::string& variableName) const;
		std::vector<std::string> getAliasInfo(const llvm::Value& pointerSource, llvm::AAResults& aliasAnalysis);

	public:
		RefinementInstructionConstraintGenerator(FixpointTypeConvertor& _fixpointTypeConvertor, VariablesEnvironment& _variableEnv) : fixpointTypeConvertor(_fixpointTypeConvertor), variableEnv(_variableEnv) {}
		ResultType CaptureBinaryOperatorConstraint(const std::string& blockName, const BinaryOperator& binaryOpInst);
		ResultType CaptureReturnInstructionConstraint(const std::string& blockName, const ReturnInst& returnInst);
		ResultType CaptureComparisonInstructionConstraint(const std::string& blockName, const CmpInst& cmpInst);
		ResultType CaptureZeroExtendInstructionConstraint(const std::string& blockName, const ZExtInst& zextInst);
		ResultType CaptureBranchInstructionConstraint(const std::string& blockName, const BranchInst& brInst);
		ResultType CapturePhiInstructionConstraint(const std::string& blockName, const PHINode& phiInst);
		ResultType CaptureSelectInstructionConstraint(const std::string& blockName, const SelectInst& selectInst);
		ResultType CaptureCallInstructionConstraint(const std::string& blockName, const CallInst& callInst, const std::string& callVariablesPrefixUsed, const RefinementFunctionSignatureInfo* callRefFunctionInfo);
		ResultType CaptureAllocaInstructionConstraint(const std::string& blockName, const AllocaInst& allocaInst);
		ResultType CaptureStoreInstructionConstraint(const std::string& blockName, const StoreInst& storeInst, llvm::AAResults& aliasAnalysis);
		ResultType CaptureLoadInstructionConstraint(const std::string& blockName, const LoadInst& loadInst, llvm::AAResults& aliasAnalysis);
		ResultType CaptureBlockConstraints(const std::string& blockName);
	};
}

#endif