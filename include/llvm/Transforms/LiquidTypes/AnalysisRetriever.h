#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_ANALYSISRETRIEVER_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_ANALYSISRETRIEVER_H

#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureInfo.h"


namespace liquid {

	class AnalysisRetriever
	{
		const std::function<RefinementFunctionSignatureInfo*(llvm::Function&)> GetAnalysisFunc;
	public:
		AnalysisRetriever(const std::function<RefinementFunctionSignatureInfo*(llvm::Function&)> getAnalysisFunc) : GetAnalysisFunc(getAnalysisFunc) {}
		const RefinementFunctionSignatureInfo* GetAnalysisForFunction(llvm::Function& function) const;
	};
}


#endif