#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSCHECKER_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSCHECKER_H

#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/LiquidFixpointBuilder/FixpointConstraintBuilder.h"

namespace llvm {

	/// A pass which verifies refinements 
	class RefinementCheckerPass : public PassInfoMixin<RefinementCheckerPass> {
	public:
		PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
	};

	Pass *createRefinementCheckerPass();

	struct RefinementCheckerParameters {
		static std::string FixpointExecutablePath;
	};
}

#endif
