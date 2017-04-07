#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadataParser.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/LiquidTypes/RefinementConstraintGenerator.h"

using namespace liquid;

namespace llvm {

	namespace {

		void runRefinementAnalysis(Function &F, RefinementFunctionInfo& r)
		{
			auto metadata = F.getMetadata("refine");
			//no refinement data
			if (metadata == nullptr)
			{
				return;
			}

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

			RefinementConstraintGenerator constraintGenerator;
			constraintGenerator.BuildConstraintsFromSignature(r.ParsedFnRefinementMetadata);
		}
	}

	char RefinementFunctionAnalysisPass::ID = 0;
	INITIALIZE_PASS(RefinementFunctionAnalysisPass, "refinementAnalysis", "Refinement constraints Construction", true, true)

	bool RefinementFunctionAnalysisPass::runOnFunction(Function &F) {
		runRefinementAnalysis(F, RI);
		return false;
	}

	AnalysisKey RefinementFunctionAnalysis::Key;
	RefinementFunctionInfo RefinementFunctionAnalysis::run(Function &F, FunctionAnalysisManager &AM)
	{
		RefinementFunctionInfo r;
		runRefinementAnalysis(F, r);
		return r;
	}
}