#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_VARIABLESENVIRONMENT_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_VARIABLESENVIRONMENT_H

#include <map>
#include <string>
#include <set>
#include <vector>
#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/FixpointType.h"
#include "llvm/Transforms/LiquidTypes/FunctionBlockGraph.h"

using namespace std::literals::string_literals;

namespace liquid
{

	class PhiNodeObligation
	{
	public:
		const std::string VariableSource;
		const std::string TargetFutureVariable;

		PhiNodeObligation() {}
		PhiNodeObligation(const std::string variableSource, const std::string targetFutureVariable) : VariableSource(variableSource), TargetFutureVariable(targetFutureVariable) {}
	};

	class VariablesEnvironment
	{
	private:
		std::string currentBlockName = ""s;
		const FunctionBlockGraph& functionBlockGraph;

		std::map<std::string, FixpointType> variableTypes;
		std::map<std::string, Counter> mutableVariableState;
		std::map<std::string, std::vector<std::string>> mutableVariableConstraints;
		std::map<std::string, std::map<std::string, std::string>> variablesMappingsPerBlock;
		std::map<std::string, std::set<std::string>> variablesValuesPerBlock;
		std::set<std::string> finishedBlocks;
		std::map<std::string, std::vector<PhiNodeObligation>> phiNodeObligations;

		std::map<std::string, std::map<std::string, bool>> booleanInformation;
		std::map<std::string, std::map<bool, std::string>> createdInfoBinders;

		std::set<std::string> outputVariables;
		std::set<std::string> mutableVariables;

		ResultType addInformation(const std::string& blockName, const std::string& variableName, bool information);
		ResultType getInformationBinders(const std::string& blockName, std::vector<std::string>& informationBinders);

		ResultType createVariable(const std::string& variable, const std::string& mappedVariableName, const FixpointType& type, const std::vector<std::string>& constraints, const std::string& expression);
		ResultType createIOVariable(const std::string& variable, const FixpointType& type, const std::vector<std::string>& constraints);
		ResultType validateFinishedAndUnfinishedBlockStates(const std::vector<std::string>& previousFinishedBlocks, const std::vector<std::string>& previousUnfinishedBlocks);
		ResultType getCommonVariables(std::vector<std::string> previousBlocks, std::set<std::string>& commonVariables);
		std::string getNextVariableName(const std::string& variable);
		std::vector<std::string> getBlockBinders(const std::string& blockName);
		ResultType createPhiNodeWithoutCreatedBinders(const std::string& variable, const std::string& mappedVariableName, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks);
		ResultType createPhiNodeInternal(const std::string& variable, const std::string& mappedVariableName, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks);
		ResultType endBlock(const std::string& blockName);

	public:
		Counter FreshState;
		FixpointConstraintBuilder constraintBuilder;

		VariablesEnvironment(const FunctionBlockGraph& _functionBlockGraph) : functionBlockGraph(_functionBlockGraph) {}
		ResultType StartBlock(const std::string& blockName);
		ResultType CreateImmutableInputVariable(const std::string& variable, const FixpointType& type, const std::vector<std::string>& constraints);
		ResultType CreateMutableOutputVariable(const std::string& variable, const FixpointType& type, const std::vector<std::string>& constraints);
		ResultType CreateImmutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression);
		ResultType CreateMutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression);
		ResultType AssignMutableVariable(std::string variable, std::string expression);
		ResultType AddBranchInformation(const std::string& booleanVariable, const bool variableValue, const std::string& targetBlock, const std::string& contraryBlock);
		ResultType CreatePhiNode(const std::string& variable, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks);
		bool IsVariableDefined(std::string variableName);
		std::string GetVariableName(std::string variableName);
		ResultType ToStringOrFailure(std::string& output);
	};
}

#endif
