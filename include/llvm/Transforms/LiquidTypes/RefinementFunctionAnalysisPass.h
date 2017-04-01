#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONANALYSISPASS_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONANALYSISPASS_H

#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"

namespace llvm {

	class RefinementFunctionInfo {
	public:
	};

	class RefinementFunctionAnalysisPass : public FunctionPass {
		RefinementFunctionInfo RI;
	public:
		static char ID; // Pass identification, replacement for typeid

		RefinementFunctionAnalysisPass() : FunctionPass(ID) {
			initializeRefinementFunctionAnalysisPassPass(*PassRegistry::getPassRegistry());
		}

		RefinementFunctionInfo &getRefinementInfo() { return RI; }
		const RefinementFunctionInfo &getRefinementInfo() const { return RI; }

		/// \brief Calculate the natural loop information for a given function.
		bool runOnFunction(Function &F) override;

		//void verifyAnalysis() const override;

		//void releaseMemory() override { RI.releaseMemory(); }

		void print(raw_ostream &O, const Module *M = nullptr) const override;

		void getAnalysisUsage(AnalysisUsage &AU) const override { AU.setPreservesAll(); }
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