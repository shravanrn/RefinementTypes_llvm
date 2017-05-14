#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSCONSTRAINTGENERATOR_H

#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/FixpointTypeConvertor.h"
#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"
#include "llvm/Transforms/LiquidTypes/RefinementInstructionConstraintGenerator.h"
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
		FixpointConstraintBuilder constraintBuilder;
		FixpointTypeConvertor fixpointTypeConvertor;
		VariablesEnvironment variableEnv;
		RefinementInstructionConstraintGenerator instructionConstraintBuilder;

		static std::string getMaxValueForIntWidth(int width);
		ResultType registerAndRetrieveIntegerQualifiers(const llvm::IntegerType& type, std::vector<std::string>& constraints);
		ResultType addConstraintsForVariable(const RefinementMetadataForVariable& variable, const std::string& prefix, const std::string& blockName, const bool ignoreAssumes, const std::map<std::string, std::string>& variableReplacements, const std::map<std::string, FixpointBaseType>& variableTypes);
		ResultType buildConstraintsFromSignatureForBlock(const RefinementMetadata& refinementData, const std::string& prefix, const std::string& blockName, const bool ignoreParameterAssumes, const bool ignoreReturnAssumes);
		ResultType generateCallSignatureVariables(const std::string& blockName, const CallInst& callInst, const AnalysisRetriever& analysisRetriever, std::string& prefixUsed, const RefinementFunctionSignatureInfo* &callFunctionInfo);
		ResultType capturePtrConstraints();

	public:
		RefinementConstraintGenerator(const Function &F, const DominatorTree& dominatorTree) : Func(F), variableEnv(F, dominatorTree), instructionConstraintBuilder(constraintBuilder, fixpointTypeConvertor, variableEnv) {}
		ResultType BuildConstraintsFromSignature(const RefinementMetadata& refinementData);
		ResultType BuildConstraintsFromInstructions(const RefinementMetadata& refinementData, const AnalysisRetriever& analysisRetriever);
		ResultType CaptureLoopConstraints(const llvm::LoopInfo& loopInfo);
		ResultType ToString(std::string& output);

	};
}

#endif