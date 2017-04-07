#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H

#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionInfo.h"

using namespace llvm;

namespace liquid {

	class RefinementConstraintGenerator
	{
	private:
		FixpointConstraintBuilder constraintBuilder;

	public:
		void BuildConstraintsFromSignature(const RefinementMetadata& refinementInfo);
	};
}

#endif