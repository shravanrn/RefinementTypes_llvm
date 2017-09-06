#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureAnalysisPass.h"

namespace liquid
{
	bool AnalysisRetriever::ContainsAnalysisForFunction(llvm::Function& function) const
	{
		return ContainsAnalysisFunc(function);
	}

	RefinementFunctionSignatureInfo* AnalysisRetriever::GetAnalysisForFunction(llvm::Function& function) const
	{
		return GetAnalysisFunc(function);
	}
}