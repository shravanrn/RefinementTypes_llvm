#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace liquid;

namespace {

	void runRefinementAnalysis(Function &F, RefinementFunctionSignatureInfo& r)
	{
		auto metadata = F.getMetadata("refinement");
		//no refinement data
		if (metadata == nullptr)
		{
			return;
		}

		{
			ResultType getRefData = RefinementMetadata_Raw::Extract(F, r.FnRefinementMetadata_Raw);
			if (!getRefData.Succeeded) { report_fatal_error(getRefData.ErrorMsg); }
		}

		{
			ResultType getRefData = RefinementMetadata::ParseMetadata(r.FnRefinementMetadata_Raw, r.ParsedFnRefinementMetadata);
			if (!getRefData.Succeeded) { report_fatal_error(getRefData.ErrorMsg); }
		}
	}
}

char RefinementFunctionSignatureAnalysisPass::ID = 0;
INITIALIZE_PASS_BEGIN(RefinementFunctionSignatureAnalysisPass, "refinementSignatureAnalysis", "Refinement Signature constraints Construction", true, true)
INITIALIZE_PASS_END(RefinementFunctionSignatureAnalysisPass, "refinementSignatureAnalysis", "Refinement Signature constraints Construction", true, true)

namespace llvm {

	bool RefinementFunctionSignatureAnalysisPass::runOnFunction(Function &F) {

		std::string key = F.getName().str();
		RI[key] = RefinementFunctionSignatureInfo();
		runRefinementAnalysis(F, RI[key]);

		return false;
	}

	void RefinementFunctionSignatureAnalysisPass::getAnalysisUsage(AnalysisUsage &AU) const {
		AU.setPreservesAll();
	}

	AnalysisKey RefinementFunctionSignatureAnalysis::Key;
	RefinementFunctionSignatureInfo RefinementFunctionSignatureAnalysis::run(Function &F, FunctionAnalysisManager &AM)
	{
		RefinementFunctionSignatureInfo r;
		std::string key = F.getName().str();
		runRefinementAnalysis(F, r);

		return r;
	}
}