#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONSIGNATUREANALYSISPASS_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONSIGNATUREANALYSISPASS_H

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureInfo.h"

#include <map>
using namespace liquid;

namespace llvm {

	class RefinementFunctionSignatureAnalysisPass : public FunctionPass {
		std::map<std::string, RefinementFunctionSignatureInfo> RI;
	public:
		static char ID; // Pass identification, replacement for typeid

		RefinementFunctionSignatureAnalysisPass() : FunctionPass(ID) {
			initializeRefinementFunctionSignatureAnalysisPassPass(*PassRegistry::getPassRegistry());
		}

		std::map<std::string, RefinementFunctionSignatureInfo> &getRefinementInfo() { return RI; }
		const std::map<std::string, RefinementFunctionSignatureInfo> &getRefinementInfo() const { return RI; }

		bool runOnFunction(Function &F) override;

		void getAnalysisUsage(AnalysisUsage &AU) const override;
	};

	/// A pass which computes refinement constraints for a function
	class RefinementFunctionSignatureAnalysis : public AnalysisInfoMixin<RefinementFunctionSignatureAnalysis> {
		friend AnalysisInfoMixin<RefinementFunctionSignatureAnalysis>;
		static AnalysisKey Key;
	public:
		typedef RefinementFunctionSignatureInfo Result;

		RefinementFunctionSignatureInfo run(Function &F, FunctionAnalysisManager &AM);
	};
}

#endif