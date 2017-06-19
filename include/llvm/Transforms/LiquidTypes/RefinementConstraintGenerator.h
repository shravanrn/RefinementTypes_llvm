#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H

#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/FixpointTypeConvertor.h"
#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidTypes/RefinementInstructionConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/FunctionBlockGraph.h"
#include "llvm/Analysis/LoopInfo.h"
#include <string>
#include <vector>

using namespace llvm;

namespace liquid {

	class AnalysisRetriever;
	class RefinementFunctionSignatureInfo;

	class RefinementConstraintGenerator
	{
	private:
		const llvm::Function& Func;
		FixpointTypeConvertor fixpointTypeConvertor;
		VariablesEnvironment variableEnv;
		RefinementInstructionConstraintGenerator instructionConstraintBuilder;

		static std::string getMaxValueForIntWidth(int width);
		ResultType registerAndRetrieveIntegerQualifiers(const llvm::IntegerType& type, std::vector<std::string>& constraints);
		ResultType addConstraintsForVariable(const RefinementMetadataForVariable& variable, const std::string& prefix, const std::string& blockName, const bool ignoreAssumes, const bool isReturnType, const std::map<std::string, std::string>& variableReplacements, const std::map<std::string, FixpointType>& variableTypes);
		ResultType buildConstraintsFromSignatureForBlock(const RefinementMetadata& refinementData, const std::string& prefix, const std::string& blockName, const bool ignoreParameterAssumes, const bool ignoreReturnAssumes);
		ResultType generateCallSignatureVariables(const std::string& blockName, const CallInst& callInst, const AnalysisRetriever& analysisRetriever, std::string& prefixUsed, const RefinementFunctionSignatureInfo* &callFunctionInfo);

	public:
		RefinementConstraintGenerator(const Function &F, const FunctionBlockGraph& functionBlockGraph) : Func(F), variableEnv(functionBlockGraph), instructionConstraintBuilder(fixpointTypeConvertor, variableEnv) {}
		ResultType BuildConstraintsFromSignature(const RefinementMetadata& refinementData);
		ResultType BuildConstraintsFromInstructions(const RefinementMetadata& refinementData, llvm::AAResults& aliasAnalysis, const AnalysisRetriever& analysisRetriever);
		ResultType CaptureLoopConstraints(const llvm::LoopInfo& loopInfo);
		ResultType ToString(std::string& output);

	};
}

#endif