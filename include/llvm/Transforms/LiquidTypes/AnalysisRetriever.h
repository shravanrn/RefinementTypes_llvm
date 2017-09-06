#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_ANALYSISRETRIEVER_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_ANALYSISRETRIEVER_H

#include "llvm/Transforms/LiquidTypes/RefinementFunctionSignatureInfo.h"


namespace liquid {

	class AnalysisRetriever
	{
		const std::function<bool(llvm::Function&)> ContainsAnalysisFunc;
		const std::function<RefinementFunctionSignatureInfo*(llvm::Function&)> GetAnalysisFunc;
	public:
		AnalysisRetriever(
			std::function<bool(llvm::Function&)> containsAnalysisFunc,
			const std::function<RefinementFunctionSignatureInfo*(llvm::Function&)> getAnalysisFunc
		) : ContainsAnalysisFunc(containsAnalysisFunc), GetAnalysisFunc(getAnalysisFunc) {}

		bool ContainsAnalysisForFunction(llvm::Function& function) const;
		RefinementFunctionSignatureInfo* GetAnalysisForFunction(llvm::Function& function) const;
	};
}


#endif