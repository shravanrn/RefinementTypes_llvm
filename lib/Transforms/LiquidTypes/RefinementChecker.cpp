#include "llvm/Transforms/LiquidTypes/RefinementChecker.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"

#include <regex>
#include <string>

using namespace llvm;

namespace liquid {

	void runRefinementPass(const Function &F, const RefinementFunctionInfo& refinementInfo)
	{
		if (refinementInfo.RefinementDataFound)
		{
			std::string constraints;
			ResultType constraintGenRet = refinementInfo.ConstraintGenerator->ToString(constraints);

			if (!constraintGenRet.Succeeded)
			{
				report_fatal_error(constraintGenRet.ErrorMsg);
			}
		}
	}
}

namespace {

	class RefinementChecker : public FunctionPass {
	public:
		static char ID; // Pass identification, replacement for typeid
		RefinementChecker() : FunctionPass(ID) {
			initializeRefinementCheckerPass(*PassRegistry::getPassRegistry());
		}

		/// Main run interface method. 
		bool runOnFunction(Function &F) override
		{
			auto& refinementInfo = getAnalysis<RefinementFunctionAnalysisPass>().getRefinementInfo();
			liquid::runRefinementPass(F, refinementInfo);
			return false;
		}

		void getAnalysisUsage(AnalysisUsage &AU) const override {
			AU.addRequired<RefinementFunctionAnalysisPass>();
			AU.setPreservesAll();
		}
	};
}

char RefinementChecker::ID = 0;
INITIALIZE_PASS_BEGIN(RefinementChecker, "refinement-checker", "Check refinement type annotations", false, false)
INITIALIZE_PASS_DEPENDENCY(RefinementFunctionAnalysisPass)
INITIALIZE_PASS_END(RefinementChecker, "refinement-checker", "Check refinement type annotations", false, false)

PreservedAnalyses RefinementCheckerPass::run(Function &F, FunctionAnalysisManager &AM)
{
	auto& refinementInfo = AM.getResult<RefinementFunctionAnalysis>(F);
	liquid::runRefinementPass(F, refinementInfo);
	return PreservedAnalyses::all();
}

Pass *llvm::createRefinementCheckerPass()
{
	return new RefinementChecker();
}
