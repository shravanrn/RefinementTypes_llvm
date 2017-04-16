#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H

#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/FixpointTypeConvertor.h"
#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidTypes/RefinementInstructionConstraintGenerator.h"
#include "llvm/Analysis/LoopInfo.h"
#include <string>
#include <vector>

using namespace llvm;

namespace liquid {

	class RefinementConstraintGenerator
	{
	private:
		const llvm::Function& Func;
		FixpointConstraintBuilder constraintBuilder;
		FixpointTypeConvertor fixpointTypeConvertor;
		VariablesEnvironment variableEnv;
		RefinementInstructionConstraintGenerator instructionConstraintBuilder;

		static std::string getMaxValueForIntWidth(int width);
		std::vector<std::string> getNonDependentConstraints(std::string variableConstraint);
		ResultType registerAndRetrieveIntegerQualifiers(const llvm::IntegerType& type, std::vector<std::string>& constraints);
		ResultType addConstraintsForVariable(const RefinementMetadataForVariable& variable, const std::string& blockName, bool ignoreAssumes);
	public:
		RefinementConstraintGenerator(const Function &F, const DominatorTree& dominatorTree) : Func(F), variableEnv(F, dominatorTree), instructionConstraintBuilder(constraintBuilder, fixpointTypeConvertor, variableEnv) {};
		ResultType BuildConstraintsFromSignature(const RefinementMetadata& refinementData);
		ResultType BuildConstraintsFromInstructions(const RefinementMetadata& refinementData);
		ResultType CaptureLoopConstraints(const llvm::LoopInfo& loopInfo);
		ResultType ToString(std::string& output);

	};
}

#endif