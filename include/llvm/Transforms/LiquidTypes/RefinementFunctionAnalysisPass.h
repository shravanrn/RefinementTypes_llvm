#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONANALYSISPASS_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONANALYSISPASS_H

#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionInfo.h"

#include <map>
using namespace liquid;

namespace llvm {

	class RefinementFunctionAnalysisPass : public FunctionPass {
		std::map<std::string, RefinementFunctionInfo> RI;
	public:
		static char ID; // Pass identification, replacement for typeid

		RefinementFunctionAnalysisPass() : FunctionPass(ID) {
			initializeRefinementFunctionAnalysisPassPass(*PassRegistry::getPassRegistry());
		}

		std::map<std::string, RefinementFunctionInfo> &getRefinementInfo() { return RI; }
		const std::map<std::string, RefinementFunctionInfo> &getRefinementInfo() const { return RI; }

		/// \brief Calculate the natural loop information for a given function.
		bool runOnFunction(Function &F) override;

		void getAnalysisUsage(AnalysisUsage &AU) const override;
	};

	/// A pass which computes refinement constraints for a function
	class RefinementFunctionAnalysis : public AnalysisInfoMixin<RefinementFunctionAnalysis> {
		friend AnalysisInfoMixin<RefinementFunctionAnalysis>;
		static AnalysisKey Key;
	public:
		typedef RefinementFunctionInfo Result;

		RefinementFunctionInfo run(Function &F, FunctionAnalysisManager &AM);
	};
}

#endif