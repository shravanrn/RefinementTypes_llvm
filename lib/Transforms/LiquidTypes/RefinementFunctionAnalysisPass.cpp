#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"

using namespace liquid;

namespace llvm {

	namespace {

		void runRefinementAnalysis(Function &F, const DominatorTree& dominatorTree, const llvm::LoopInfo& loopInfo, const AnalysisRetriever& analysisRetriever, RefinementFunctionInfo& r)
		{
			auto metadata = F.getMetadata("refinement");
			//no refinement data
			if (metadata == nullptr)
			{
				return;
			}

			r.RefinementDataFound = true;

			{
				if (!analysisRetriever.ContainsAnalysisForFunction(F)) { report_fatal_error("Refinement Types : Expected to find signature analysis"); }
				r.SignatureMetadata = analysisRetriever.GetAnalysisForFunction(F);
			}
			
			//if we have the function body
			if (!F.isDeclaration())
			{
				r.ConstraintGenerator = std::make_unique<RefinementConstraintGenerator>(F, dominatorTree);

				{
					ResultType constraintRes = r.ConstraintGenerator->BuildConstraintsFromSignature(r.SignatureMetadata->ParsedFnRefinementMetadata);
					if (!constraintRes.Succeeded) { report_fatal_error(constraintRes.ErrorMsg); }
				}

				{
					ResultType constraintRes = r.ConstraintGenerator->BuildConstraintsFromInstructions(r.SignatureMetadata->ParsedFnRefinementMetadata, analysisRetriever);
					if (!constraintRes.Succeeded) { report_fatal_error(constraintRes.ErrorMsg); }
				}

				{
					ResultType constraintRes = r.ConstraintGenerator->CaptureLoopConstraints(loopInfo);
					if (!constraintRes.Succeeded) { report_fatal_error(constraintRes.ErrorMsg); }
				}
			}
		}
	}

	char RefinementFunctionAnalysisPass::ID = 0;
	INITIALIZE_PASS_BEGIN(RefinementFunctionAnalysisPass, "refinementAnalysis", "Refinement constraints Construction", true, true)
	INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
	INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
	INITIALIZE_PASS_DEPENDENCY(RefinementFunctionSignatureAnalysisPass)
	INITIALIZE_PASS_END(RefinementFunctionAnalysisPass, "refinementAnalysis", "Refinement constraints Construction", true, true)

	bool RefinementFunctionAnalysisPass::runOnModule(Module &M) {
		std::map<std::string, RefinementFunctionSignatureInfo> sigInfo;

		for (auto& F : M.functions())
		{
			sigInfo = getAnalysis<RefinementFunctionSignatureAnalysisPass>(F).getRefinementInfo();
		}

		for (auto& F : M.functions())
		{
			auto& dominatorTree = getAnalysis<DominatorTreeWrapperPass>(F).getDomTree();
			auto& loopInfo = getAnalysis<LoopInfoWrapperPass>(F).getLoopInfo();

			std::string key = F.getName().str();
			RI[key] = RefinementFunctionInfo();

			auto containsFunc = [&sigInfo](llvm::Function& f) {
				std::string key = f.getName().str();
				return RefinementUtils::containsKey(sigInfo, key);
			};

			auto retrieverFunc = [&sigInfo](llvm::Function& f) {
				std::string key = f.getName().str();
				return &(sigInfo.at(key));
			};

			AnalysisRetriever analysisRetriever(containsFunc, retrieverFunc);
			runRefinementAnalysis(F, dominatorTree, loopInfo, analysisRetriever, RI[key]);
		}
		return false;
	}

	void RefinementFunctionAnalysisPass::getAnalysisUsage(AnalysisUsage &AU) const {
		AU.addRequired<DominatorTreeWrapperPass>();
		AU.addRequired<LoopInfoWrapperPass>();
		AU.addRequired<RefinementFunctionSignatureAnalysisPass>();
		AU.setPreservesAll();
	}

	AnalysisKey RefinementFunctionAnalysis::Key;

	RefinementFunctionInfo RefinementFunctionAnalysis::run(Function &F, FunctionAnalysisManager &AM)
	{
		RefinementFunctionInfo r;
		auto& dominatorTree = AM.getResult<DominatorTreeAnalysis>(F);
		auto& loopInfo = AM.getResult<LoopAnalysis>(F);
		auto refinementSignatureInfo = AM.getResult<RefinementFunctionSignatureAnalysis>(F);

		auto containsFunc = [&AM](llvm::Function& f) {
			std::string key = f.getName().str();
			return AM.getCachedResult<RefinementFunctionSignatureAnalysis>(f) != nullptr;
		};

		auto retrieverFunc = [&AM](llvm::Function& f) {
			return &(AM.getResult<RefinementFunctionSignatureAnalysis>(f));
		};

		AnalysisRetriever analysisRetriever(containsFunc, retrieverFunc);
		runRefinementAnalysis(F, dominatorTree, loopInfo, analysisRetriever, r);

		return r;
	}
}