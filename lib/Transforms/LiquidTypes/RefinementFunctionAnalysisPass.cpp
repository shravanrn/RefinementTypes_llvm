#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Pass.h"

namespace llvm {

	char RefinementFunctionAnalysisPass::ID = 0;
	INITIALIZE_PASS(RefinementFunctionAnalysisPass, "refinementAnalysis",
		"Refinement constraints Construction", true, true)

	bool RefinementFunctionAnalysisPass::runOnFunction(Function &F) {
		//DT.recalculate(F);
		return false;
	}

	void RefinementFunctionAnalysisPass::print(raw_ostream &OS, const Module *) const {
		//DT.print(OS);
	}


	AnalysisKey RefinementFunctionAnalysis::Key;
	RefinementFunctionInfo RefinementFunctionAnalysis::run(Function &F, FunctionAnalysisManager &AM)
	{
		RefinementFunctionInfo r;
		return r;
	}
}