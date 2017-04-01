#include "llvm/Transforms/LiquidTypes/RefinementChecker.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include <regex>
#include <string>

using namespace llvm;

namespace liquid {

	void runRefinementPass(const Function &F, const DominatorTree& dominatorTree, const llvm::LoopInfo& loopInfo, const RefinementFunctionInfo& refinementInfo)
	{

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
			auto& dominatorTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
			auto& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
			auto& refinementInfo = getAnalysis<RefinementFunctionAnalysisPass>().getRefinementInfo();
			liquid::runRefinementPass(F, dominatorTree, loopInfo, refinementInfo);
			return false;
		}

		void getAnalysisUsage(AnalysisUsage &AU) const override {
			AU.addRequired<DominatorTreeWrapperPass>();
			AU.addRequired<LoopInfoWrapperPass>();
			AU.addRequired<RefinementFunctionAnalysisPass>();
			AU.setPreservesAll();
		}
	};
}

char RefinementChecker::ID = 0;
INITIALIZE_PASS_BEGIN(RefinementChecker, "refinement-checker", "Check refinement type annotations", false, false)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(RefinementFunctionAnalysisPass)
INITIALIZE_PASS_END(RefinementChecker, "refinement-checker", "Check refinement type annotations", false, false)

PreservedAnalyses RefinementCheckerPass::run(Function &F, FunctionAnalysisManager &AM)
{
	auto& dominatorTree = AM.getResult<DominatorTreeAnalysis>(F);
	auto& loopInfo = AM.getResult<LoopAnalysis>(F);
	auto& refinementInfo = AM.getResult<RefinementFunctionAnalysis>(F);
	liquid::runRefinementPass(F, dominatorTree, loopInfo, refinementInfo);
	return PreservedAnalyses::all();
}

Pass *llvm::createRefinementCheckerPass()
{
	return new RefinementChecker();
}
