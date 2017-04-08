#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H

#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include <string>
#include <vector>

using namespace llvm;

namespace liquid {

	class RefinementConstraintGenerator
	{
	private:
		FixpointConstraintBuilder constraintBuilder;
		VariablesEnvironment variableEnv;

		static std::string getMaxValueForIntWidth(int width);
		std::vector<std::string> getNonDependentConstraints(std::string variableConstraint);
		std::vector<std::string> registerAndRetrieveIntegerQualifiers(const llvm::IntegerType& type);
		ResultType addConstraintsForVariable(const RefinementMetadataForVariable& variable, const std::string& blockName, bool ignoreAssumes);
	public:
		RefinementConstraintGenerator(const Function &F, const DominatorTree& dominatorTree) : variableEnv(F, dominatorTree) {};
		ResultType BuildConstraintsFromSignature(const RefinementMetadata& refinementData);
		ResultType ToString(std::string& output);

	};
}

#endif