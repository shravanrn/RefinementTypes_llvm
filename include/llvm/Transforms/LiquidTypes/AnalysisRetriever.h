#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_ANALYSISRETRIEVER_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_ANALYSISRETRIEVER_H

#include "llvm/Transforms/LiquidTypes/RefinementFunctionInfo.h"

#include <map>
#include <string>

namespace liquid {

	class AnalysisRetriever
	{
	private:
		const bool UseMap;
		const std::map<std::string, RefinementFunctionInfo>* AnalysisMap;
		const FunctionAnalysisManager* AnalysisManager;
		const Function* CurrFn;
		const RefinementFunctionInfo* CurrFnRefinementInfo;

	public:
		AnalysisRetriever(std::map<std::string, RefinementFunctionInfo>* analysisMap) : UseMap(true), AnalysisMap(analysisMap), AnalysisManager(nullptr), CurrFn(nullptr), CurrFnRefinementInfo(nullptr) {}
		AnalysisRetriever(FunctionAnalysisManager* am, Function* f, RefinementFunctionInfo* refinementInfo) : UseMap(false), AnalysisMap(nullptr), AnalysisManager(am), CurrFn(f), CurrFnRefinementInfo(refinementInfo) {}

		bool ContainsAnalysisForFunction(llvm::Function& function) const;
		const RefinementFunctionInfo* GetAnalysisForFunction(llvm::Function& function) const;
	};

}


#endif