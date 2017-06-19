#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"

using namespace liquid;

namespace llvm {

	namespace {

		class LLVMFunctionBlockGraph : public FunctionBlockGraph
		{
			Function& F;
			const DominatorTree& dominatorTree;

			const llvm::BasicBlock* getBasicBlockRef(const std::string& blockName) const
			{
				for (const auto& block : F)
				{
					if (block.getName().str() == blockName)
					{
						return &block;
					}
				}

				return nullptr;
			}

		public:
			LLVMFunctionBlockGraph(Function &_F, const DominatorTree& _dominatorTree) : F(_F), dominatorTree(_dominatorTree) {}

			// Inherited via FunctionBlockGraph
			std::string GetStartingBlockName() const override
			{
				return "entry"s;
			}
			
			ResultType GetSuccessorBlocks(const std::string& blockName, std::vector<std::string>& successorBlocks) const override
			{
				const BasicBlock* blockRef = getBasicBlockRef(blockName);
				if (blockRef == nullptr)
				{
					return ResultType::Error("Block : "s + blockName + " not found"s);
				}

				auto successors = blockRef->getTerminator()->successors();
				
				for (const auto& successor : successors)
				{
					successorBlocks.push_back(successor->getName().str());
				}

				return ResultType::Success();
			}
			
			ResultType StrictlyDominates(const std::string& firstblockName, const std::string& secondBlockName, bool &result) const override
			{
				const BasicBlock* firstBlockRef = getBasicBlockRef(firstblockName);
				if (firstBlockRef == nullptr)
				{
					return ResultType::Error("Block : "s + firstblockName + " not found"s);
				}

				const BasicBlock* secondBlockRef = getBasicBlockRef(secondBlockName);
				if (secondBlockRef == nullptr)
				{
					return ResultType::Error("Block : "s + secondBlockName + " not found"s);
				}

				result = dominatorTree.properlyDominates(firstBlockRef, secondBlockRef);
				return ResultType::Success();
			}
		};

		void runRefinementAnalysis(Function &F, 
			const llvm::DominatorTree& dominatorTree,
			const llvm::LoopInfo& loopInfo, 
			llvm::AAResults& aliasAnalysis,
			const AnalysisRetriever& analysisRetriever, 
			RefinementFunctionInfo& r)
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
				LLVMFunctionBlockGraph llvmFunctionBlockGraph(F, dominatorTree);
				r.ConstraintGenerator = std::make_unique<RefinementConstraintGenerator>(F, llvmFunctionBlockGraph);

				{
					ResultType constraintRes = r.ConstraintGenerator->BuildConstraintsFromSignature(r.SignatureMetadata->ParsedFnRefinementMetadata);
					if (!constraintRes.Succeeded) { report_fatal_error(constraintRes.ErrorMsg); }
				}

				{
					ResultType constraintRes = r.ConstraintGenerator->BuildConstraintsFromInstructions(r.SignatureMetadata->ParsedFnRefinementMetadata, aliasAnalysis, analysisRetriever);
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
	INITIALIZE_PASS_DEPENDENCY(AAResultsWrapperPass)
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
			auto& aliasAnalysis = getAnalysis<AAResultsWrapperPass>(F).getAAResults();

			std::string key = F.getName().str();
			RI[key] = RefinementFunctionInfo();

			auto containsFunc = [&sigInfo](llvm::Function& f) {
				std::string key = f.getName().str();
				return RefinementUtils::ContainsKey(sigInfo, key);
			};

			auto retrieverFunc = [&sigInfo](llvm::Function& f) {
				std::string key = f.getName().str();
				return &(sigInfo.at(key));
			};

			AnalysisRetriever analysisRetriever(containsFunc, retrieverFunc);
			runRefinementAnalysis(F, dominatorTree, loopInfo, aliasAnalysis, analysisRetriever, RI[key]);
		}
		return false;
	}

	void RefinementFunctionAnalysisPass::getAnalysisUsage(AnalysisUsage &AU) const {
		AU.addRequired<DominatorTreeWrapperPass>();
		AU.addRequired<LoopInfoWrapperPass>();
		AU.addRequired<AAResultsWrapperPass>();
		AU.addRequired<RefinementFunctionSignatureAnalysisPass>();
		AU.setPreservesAll();
	}

	AnalysisKey RefinementFunctionAnalysis::Key;

	RefinementFunctionInfo RefinementFunctionAnalysis::run(Function &F, FunctionAnalysisManager &AM)
	{
		RefinementFunctionInfo r;
		auto& dominatorTree = AM.getResult<DominatorTreeAnalysis>(F);
		auto& loopInfo = AM.getResult<LoopAnalysis>(F);
		auto& aliasAnalysis = AM.getResult<AAManager>(F);
		auto refinementSignatureInfo = AM.getResult<RefinementFunctionSignatureAnalysis>(F);

		auto containsFunc = [&AM](llvm::Function& f) {
			std::string key = f.getName().str();
			return AM.getCachedResult<RefinementFunctionSignatureAnalysis>(f) != nullptr;
		};

		auto retrieverFunc = [&AM](llvm::Function& f) {
			return &(AM.getResult<RefinementFunctionSignatureAnalysis>(f));
		};

		AnalysisRetriever analysisRetriever(containsFunc, retrieverFunc);
		runRefinementAnalysis(F, dominatorTree, loopInfo, aliasAnalysis, analysisRetriever, r);

		return r;
	}
}