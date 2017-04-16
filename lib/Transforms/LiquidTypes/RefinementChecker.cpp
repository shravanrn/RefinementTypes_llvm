#include "llvm/Transforms/LiquidTypes/RefinementChecker.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/FixpointManager.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/CommandLine.h"

#include <regex>
#include <string>
#include <iostream>

using namespace llvm;

namespace liquid {

	void runRefinementPass(const Function &F, const RefinementFunctionInfo& refinementInfo)
	{
		if (refinementInfo.RefinementDataFound)
		{
			std::string constraints;
			{
				ResultType constraintGenRet = refinementInfo.ConstraintGenerator->ToString(constraints);
				if (!constraintGenRet.Succeeded)
				{
					report_fatal_error(constraintGenRet.ErrorMsg);
				}
			}

			FixpointManager fixpointManager(RefinementCheckerParameters::FixpointExecutablePath);

			FixpointResponse response;
			{
				ResultType fixpointCallRes = fixpointManager.Execute(constraints, response);
				if (!fixpointCallRes.Succeeded)
				{
					report_fatal_error(fixpointCallRes.ErrorMsg);
				}
			}

			std::cout << "Fixpoint Constraints:\n" << response.OutputStreamText << "\n";

			if (!response.IsSafe)
			{
				std::string errorStr = "Refinement Types: Could not show safety of function : " + F.getName().str();
				report_fatal_error(errorStr);
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
			liquid::runRefinementPass(F, refinementInfo[F.getName().str()]);
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

static cl::opt<std::string, true>
FixpointExecutablePath("fixpoint-path", 
	cl::NotHidden,
	cl::ValueRequired,
	cl::desc("The full path to the fixpoint executable"),
	cl::location(RefinementCheckerParameters::FixpointExecutablePath));
std::string RefinementCheckerParameters::FixpointExecutablePath;

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
