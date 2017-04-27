#include "llvm/Transforms/LiquidTypes/AnalysisRetriever.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureAnalysisPass.h"

namespace liquid
{
	const RefinementFunctionSignatureInfo* AnalysisRetriever::GetAnalysisForFunction(llvm::Function& function) const
	{
		return GetAnalysisFunc(function);
	}
}