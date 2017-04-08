#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadataParser.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementConstraintGenerator.h"
#include "llvm/Pass.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace liquid;

namespace llvm {

	namespace {

		void runRefinementAnalysis(Function &F, const DominatorTree& dominatorTree, const llvm::LoopInfo& loopInfo, RefinementFunctionInfo& r)
		{
			auto metadata = F.getMetadata("refine");
			//no refinement data
			if (metadata == nullptr)
			{
				return;
			}

			r.RefinementDataFound = true;

			{
				ResultType getRefData = RefinementMetadata::Extract(F, r.FnRefinementMetadata);

				if (!getRefData.Succeeded)
				{
					report_fatal_error(getRefData.ErrorMsg);
				}
			}

			{
				ResultType getRefData = RefinementMetadataParser::ParseMetadata(r.FnRefinementMetadata, r.ParsedFnRefinementMetadata);

				if (!getRefData.Succeeded)
				{
					report_fatal_error(getRefData.ErrorMsg);
				}
			}

			r.ConstraintGenerator = std::make_unique<RefinementConstraintGenerator>(F, dominatorTree);
			r.ConstraintGenerator->BuildConstraintsFromSignature(r.ParsedFnRefinementMetadata);
		}
	}

	char RefinementFunctionAnalysisPass::ID = 0;
	INITIALIZE_PASS_BEGIN(RefinementFunctionAnalysisPass, "refinementAnalysis", "Refinement constraints Construction", true, true)
	INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
	INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
	INITIALIZE_PASS_END(RefinementFunctionAnalysisPass, "refinementAnalysis", "Refinement constraints Construction", true, true)

	bool RefinementFunctionAnalysisPass::runOnFunction(Function &F) {
		auto& dominatorTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
		auto& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
		runRefinementAnalysis(F, dominatorTree, loopInfo, RI);
		return false;
	}

	void RefinementFunctionAnalysisPass::getAnalysisUsage(AnalysisUsage &AU) const {
		AU.addRequired<DominatorTreeWrapperPass>();
		AU.addRequired<LoopInfoWrapperPass>();
		AU.setPreservesAll();
	}

	AnalysisKey RefinementFunctionAnalysis::Key;
	RefinementFunctionInfo RefinementFunctionAnalysis::run(Function &F, FunctionAnalysisManager &AM)
	{
		RefinementFunctionInfo r;
		auto& dominatorTree = AM.getResult<DominatorTreeAnalysis>(F);
		auto& loopInfo = AM.getResult<LoopAnalysis>(F);
		runRefinementAnalysis(F, dominatorTree, loopInfo, r);

		return r;
	}
}