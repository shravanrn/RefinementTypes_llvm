#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionAnalysisPass.h"

namespace liquid
{
	bool AnalysisRetriever::ContainsAnalysisForFunction(llvm::Function& function) const
	{
		std::string inpFnName = function.getName().str();
		if (UseMap)
		{
			return RefinementUtils::containsKey(*AnalysisMap, inpFnName);
		}

		std::string currFnName = CurrFn->getName().str();
		if (inpFnName == currFnName) { return true; }

		auto cachedResult = AnalysisManager->getCachedResult<RefinementFunctionAnalysis>(function);
		return cachedResult != nullptr;
	}

	const RefinementFunctionInfo* AnalysisRetriever::GetAnalysisForFunction(llvm::Function& function) const
	{
		std::string inpFnName = function.getName().str();
		if (UseMap)
		{
			return &((*AnalysisMap).at(inpFnName));
		}

		std::string currFnName = CurrFn->getName().str();
		if (inpFnName == currFnName) { return CurrFnRefinementInfo; }

		auto cachedResult = AnalysisManager->getCachedResult<RefinementFunctionAnalysis>(function);
		assert(cachedResult != nullptr);
		return cachedResult;
	}
}