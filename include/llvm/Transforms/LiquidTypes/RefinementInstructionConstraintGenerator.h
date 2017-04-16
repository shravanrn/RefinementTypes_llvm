#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSINSTRUCTIONCONSTRAINTGENERATOR_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSINSTRUCTIONCONSTRAINTGENERATOR_H

#include <string>
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/FixpointTypeConvertor.h"
#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"

using namespace llvm;

namespace liquid {
	
	class RefinementInstructionConstraintGenerator
	{
	private:
		FixpointConstraintBuilder& constraintBuilder;
		FixpointTypeConvertor& fixpointTypeConvertor;
		VariablesEnvironment& variableEnv;

		bool isLLVMRegister(const llvm::Value& value);
		ResultType getBinderName(const llvm::Value& value, std::string& binderName);
		std::string getSignedIntFromUnsignedRepresentation(const std::string& unsignedValStr, unsigned int width);

		std::string RefinementInstructionConstraintGenerator::GetIntegerOperatorExpression(
			const BinaryOperator& binaryOpInst,
			const std::string& left,
			const std::string& operatorString,
			const std::string& right
		);

	public:
		RefinementInstructionConstraintGenerator(FixpointConstraintBuilder& _constraintBuilder, FixpointTypeConvertor& _fixpointTypeConvertor, VariablesEnvironment& _variableEnv) : constraintBuilder(_constraintBuilder), fixpointTypeConvertor(_fixpointTypeConvertor), variableEnv(_variableEnv) {}
		ResultType CaptureBinaryOperatorConstraint(const std::string& blockName, const BinaryOperator& binaryOpInst);
		ResultType CaptureReturnInstructionConstraint(const std::string& blockName, const ReturnInst& returnInst);
		ResultType CaptureComparisonInstructionConstraint(const std::string& blockName, const CmpInst& cmpInst);
		ResultType CaptureZeroExtendInstructionConstraint(const std::string& blockName, const ZExtInst& zextInst);
		ResultType CaptureBranchInstructionConstraint(const std::string& blockName, const BranchInst& brInst);
		ResultType CapturePhiInstructionConstraint(const std::string& blockName, const PHINode& phiInst);
		ResultType CaptureSelectInstructionConstraint(const std::string& blockName, const SelectInst& selectInst);
		ResultType CaptureCallInstructionConstraint(const std::string& blockName, const CallInst& callInst, const RefinementMetadata& refinementData);
	};
}

#endif