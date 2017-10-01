#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include <algorithm>
#include <cassert>

using namespace std::literals::string_literals;

namespace liquid
{
	bool VariablesEnvironment::IsVariableDefined(std::string variableName)
	{
		return RefinementUtils::ContainsKey(variableTypes, variableName);
	}

	std::string VariablesEnvironment::getNextVariableName(const std::string& variable)
	{
		std::string newVarName = variable + "__"s + mutableVariableState[variable].GetNextIdString();
		return newVarName;
	}

	std::string VariablesEnvironment::GetVariableName(std::string variableName)
	{
		assert(RefinementUtils::ContainsKey(variablesMappingsPerBlock, currentBlockName));
		assert(RefinementUtils::ContainsKey(variablesMappingsPerBlock[currentBlockName], variableName));
		return variablesMappingsPerBlock[currentBlockName][variableName];
	}

	std::string VariablesEnvironment::GetVariableAddress(std::string variableName)
	{
		return constraintBuilder.GetBinderAddress(variableName);
	}

	std::vector<std::string> VariablesEnvironment::getBlockBinders(const std::string& blockName)
	{
		auto currBinders = variablesValuesPerBlock[blockName];
		std::vector<std::string> ret(currBinders.begin(), currBinders.end());
		ret.emplace_back("__block__"s + blockName);
		return ret;
	}

	void insertOrAssignVarType(std::map<std::string, FixpointType>& mapToUse, const std::string& variable, const FixpointType& type)
	{
		auto search = mapToUse.find(variable);
		if(search != mapToUse.end()) {
			mapToUse.erase(search);
	    }
	    mapToUse.emplace(variable, type);
    }

	ResultType VariablesEnvironment::createIOVariable(
		const std::string& variable,
		const FixpointType& type,
		const std::vector<std::string>& constraints)
	{
		if (RefinementUtils::ContainsKey(variableTypes, variable)) { return ResultType::Error("Variable"s + variable + " already exists."s); }

		std::string mappedVariableName = variable;

		{
			auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(mappedVariableName, type, constraints);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		insertOrAssignVarType(variableTypes, variable, type);
		variablesMappingsPerBlock[currentBlockName][variable] = mappedVariableName;
		variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::CreateImmutableInputVariable(
		const std::string& variable,
		const FixpointType& type,
		const std::vector<std::string>& constraints)
	{
		auto createRes = createIOVariable(variable, type, constraints);
		if (!createRes.Succeeded) { return createRes; }

		//Since, this is an input variable make it accessible to every block
		auto internalVarName = GetVariableName(variable);

		for (auto& blockVarMap : variablesMappingsPerBlock)
		{
			if (!RefinementUtils::ContainsKey(blockVarMap.second, variable))
			{
				blockVarMap.second[variable] = internalVarName;
				variablesValuesPerBlock[blockVarMap.first].emplace(internalVarName);
			}
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::CreateMutableOutputVariable(
		const std::string& variable,
		const FixpointType& type,
		const std::vector<std::string>& constraints)
	{
		mutableVariableConstraints[variable] = constraints;
		outputVariables.emplace(variable);
		return createIOVariable(variable, type, constraints);
	}

	ResultType VariablesEnvironment::createVariable(
		const std::string& variable,
		const std::string& mappedVariableName,
		const FixpointType& type,
		const std::vector<std::string>& constraints,
		const std::string& expression)
	{
		auto currVariablesAndInfo = getBlockBinders(currentBlockName);

		{
			auto createBinderRes = constraintBuilder.CreateBinderWithUnknownType(mappedVariableName, type, currVariablesAndInfo);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		if (constraints.size() > 0)
		{
			std::string constraintName = "Variable_"s + mappedVariableName + "_ConstraintCheck"s;
			auto addConstRes = constraintBuilder.AddConstraint(constraintName, type, constraints, expression, currVariablesAndInfo);
			if (!addConstRes.Succeeded) { return addConstRes; }
		}

		{
			std::string constraintName = "Variable_"s + mappedVariableName + "_Assignment"s;
			auto addConstRes = constraintBuilder.AddConstraintForAssignment(constraintName, mappedVariableName, expression, currVariablesAndInfo);
			if (!addConstRes.Succeeded) { return addConstRes; }
		}

		insertOrAssignVarType(variableTypes, variable, type);
		variablesMappingsPerBlock[currentBlockName][variable] = mappedVariableName;
		variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::CreateImmutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression)
	{
		if (RefinementUtils::ContainsKey(variableTypes, variable)) { return ResultType::Error("Variable"s + variable + " already exists."s); }
		return createVariable(variable, variable, type, constraints, expression);
	}

	ResultType VariablesEnvironment::CreateMutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression)
	{
		if (RefinementUtils::ContainsKey(variableTypes, variable)) { return ResultType::Error("Variable"s + variable + " already exists."s); }
		mutableVariableConstraints[variable] = constraints;
		mutableVariables.emplace(variable);
		return createVariable(variable, variable, type, constraints, expression);
	}

	ResultType VariablesEnvironment::AssignMutableVariable(std::string variable, std::string expression)
	{
		if (outputVariables.find(variable) == outputVariables.end() && mutableVariables.find(variable) == mutableVariables.end())
		{
			return ResultType::Error("Trying to assign an immutable variable: "s + variable);
		}

		std::string newVarName = getNextVariableName(variable);
		auto varType = variableTypes.at(variable);
		return createVariable(variable, newVarName, varType, mutableVariableConstraints[variable], expression);
	}

	ResultType VariablesEnvironment::AddJumpInformation(const std::string& targetBlock)
	{
		std::string transitionGuardName = "__transition__"s + currentBlockName + "__"s + targetBlock;

		{
			auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(transitionGuardName, FixpointType::GetBoolType(), { "true"s });
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::AddBranchInformation(const std::string& booleanVariable, const bool variableValue, const std::string& targetBlock)
	{
		if (!RefinementUtils::ContainsKey(variableTypes, booleanVariable))
		{
			return ResultType::Error("Missing variable: "s + booleanVariable);
		}

		if (variableTypes.at(booleanVariable) != FixpointType::GetBoolType())
		{
			return ResultType::Error("Expected boolean type for variable: "s + booleanVariable);
		}

		const std::string assignedExpr = "__value <=> "s + (variableValue ? ""s : "~"s)  + booleanVariable;
		const std::string transitionGuardName = "__transition__"s + currentBlockName + "__"s + targetBlock;

		{
			auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(transitionGuardName, FixpointType::GetBoolType(), { assignedExpr });
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::validateFinishedAndUnfinishedBlockStates(const std::vector<std::string>& previousFinishedBlocks, const std::vector<std::string>& previousUnfinishedBlocks)
	{
		if (previousFinishedBlocks.size() == 0)
		{
			return ResultType::Error("Blocks were not completed in the right order. Expected completion of at least one of block "s + RefinementUtils::StringJoin(", "s, previousUnfinishedBlocks));
		}
		else if (previousFinishedBlocks.size() > 1)
		{
			return ResultType::Error("Blocks were not completed in the right order. Multiple completed blocks were not expected "s + RefinementUtils::StringJoin(", "s, previousFinishedBlocks));
		}

		//make sure all previous finished blocks dominate the previous unfinished blocks
		//it is not clear, that we will run into control graphs where this won't be true
		for (const auto& previousUnfinishedBlock : previousUnfinishedBlocks)
		{
			bool allDominated = true;
			for (const auto& previousFinishedBlock : previousFinishedBlocks)
			{
				bool dominated;
				{
					auto domRes = functionBlockGraph.StrictlyDominates(previousFinishedBlock, previousUnfinishedBlock, dominated);
					if (!domRes.Succeeded) { return domRes; }
				}

				if (!dominated)
				{
					allDominated = false;
					break;
				}
			}

			if (!allDominated)
			{
				return ResultType::Error("Blocks were not completed in the right order. Unexpected control flow graph. Finished: "s
					+ RefinementUtils::StringJoin(", "s, previousFinishedBlocks)
					+ " . Unfinished: "s
					+ RefinementUtils::StringJoin(", "s, previousUnfinishedBlocks)
				);
			}
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::getCommonVariables(std::vector<std::string> previousBlocks, std::set<std::string>& commonVariables)
	{
		std::vector<std::string> previousFinishedBlocks;
		std::vector<std::string> previousUnfinishedBlocks;

		for (auto& previousBlock : previousBlocks)
		{
			RefinementUtils::Contains(finishedBlocks, previousBlock) ?
				previousFinishedBlocks.push_back(previousBlock)
				:previousUnfinishedBlocks.push_back(previousBlock);
		}

		//For control flow graphs in Loops, it is possible that we haven't finished processing all previous blocks
		//In this scenario we need to create phi nodes to take care of this
		if (previousUnfinishedBlocks.size() > 0)
		{
			{
				auto validateRes = validateFinishedAndUnfinishedBlockStates(previousFinishedBlocks, previousUnfinishedBlocks);
				if (!validateRes.Succeeded) { return validateRes; }
			}

			//for now we only handle the case that we have a single finished block
			//this could be extended if needed

			auto previousFinishedBlock = previousFinishedBlocks[0];
			auto finishedBlockVariables = RefinementUtils::GetKeysSet(variablesMappingsPerBlock[previousFinishedBlock]);
			auto finishedBlockMutableVariables = RefinementUtils::SetIntersection(finishedBlockVariables, mutableVariables);

			for(const auto& finishedBlockVariable : finishedBlockMutableVariables)
			{
				std::vector<std::string> phiVariableNames;

				for (const auto& previousUnfinishedBlock : previousUnfinishedBlocks)
				{
					std::string phiVarName = "__phi__"s + previousUnfinishedBlock + "__"s + finishedBlockVariable;
					phiVariableNames.push_back(phiVarName);

					PhiNodeObligation obligation(finishedBlockVariable, phiVarName);
					phiNodeObligations[previousUnfinishedBlock].push_back(obligation);
				}

				auto allBlocks = RefinementUtils::VectorAppend({ previousFinishedBlock }, previousUnfinishedBlocks);
				auto allPhiVars = RefinementUtils::VectorAppend({ finishedBlockVariable }, phiVariableNames);

				std::string newVarName = getNextVariableName(finishedBlockVariable);
				{
					auto phiRes = createPhiNodeInternal(finishedBlockVariable, newVarName, variableTypes[finishedBlockVariable], allPhiVars, allBlocks);
					if (!phiRes.Succeeded) { return phiRes; }
				}
			}

			auto finishedBlockImmutableVariables = RefinementUtils::SetDifference(RefinementUtils::SetDifference(finishedBlockVariables, mutableVariables), outputVariables);
			for (auto& immutableVar : finishedBlockImmutableVariables)
			{
				for (const auto& previousUnfinishedBlock : previousUnfinishedBlocks)
				{
					variablesMappingsPerBlock[previousUnfinishedBlock][immutableVar] = variablesMappingsPerBlock[previousFinishedBlock][immutableVar];
					variablesValuesPerBlock[previousUnfinishedBlock].emplace(variablesMappingsPerBlock[previousFinishedBlock][immutableVar]);
				}
			}
		}

		bool first = true;

		for (auto& previousBlock : previousBlocks)
		{
			if (first)
			{
				commonVariables = RefinementUtils::GetKeysSet(variablesMappingsPerBlock[previousBlock]);
				first = false;
			}
			else
			{
				std::set<std::string> currBlockVars = RefinementUtils::GetKeysSet(variablesMappingsPerBlock[previousBlock]);
				commonVariables = RefinementUtils::SetIntersection(commonVariables, currBlockVars);
			}
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::getBlockBindersForPhiNode(const std::string& previousBlock, const std::string& blockName, const std::string& mappedVariableName, std::vector<std::string>& binders)
	{
		binders = getBlockBinders(previousBlock);

		//strengthen the environment with the transition edge
		std::string transitionGuardName = "__transition__"s + previousBlock + "__"s + currentBlockName;
		binders.emplace_back(transitionGuardName);

		std::string predecessorEntryGuard;
		{
			auto getPredEntryRes = getBlockGuard(previousBlock, predecessorEntryGuard);
			if (!getPredEntryRes.Succeeded) { return getPredEntryRes; }
		}

		std::string phiNodeTransitionGuard = predecessorEntryGuard + " && "s + transitionGuardName;
		std::string phiNodeGuardName = "__phi__"s + mappedVariableName + "_"s + previousBlock;

		{
			auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(phiNodeGuardName, FixpointType::GetBoolType(), { phiNodeTransitionGuard });
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		binders.emplace_back(phiNodeGuardName);
		return ResultType::Success();
	}

	ResultType VariablesEnvironment::createPhiNodeWithoutCreatedBinders(const std::string& variable, const std::string& mappedVariableName, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks)
	{
		if (sourceVariableNames.size() != previousBlocks.size())
		{
			return ResultType::Error("Expected phi node variables and associated block size to be the same"s);
		}

		{
			auto currVariablesAndInfo = getBlockBinders(currentBlockName);
			auto createBinderRes = constraintBuilder.CreateBinderWithUnknownType(mappedVariableName, type, currVariablesAndInfo);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		for (size_t i = 0, csize = previousBlocks.size(); i < csize; i++)
		{
			auto& previousBlock = previousBlocks[i];
			std::vector<std::string> blockVariablesAndInfo;
			{
				auto getBindersRes = getBlockBindersForPhiNode(previousBlock, currentBlockName, mappedVariableName, blockVariablesAndInfo);
				if (!getBindersRes.Succeeded) { return getBindersRes; }
			}

			auto blockVariableMapping = sourceVariableNames[i];
			//ensure that the future binder is part of the info
			if (!RefinementUtils::Contains(blockVariablesAndInfo, blockVariableMapping))
			{
				blockVariablesAndInfo.push_back(blockVariableMapping);
			}

			std::string constraintName = "Variable_"s + previousBlock + "_"s + currentBlockName + "_"s + mappedVariableName;

			//hack - for variables that aren't created yet aka future binders, there will be no mapping, so we will just use the name as is
			//we are relying on the fact, that the first variable mapping is the same as the variable name and that future binders are immutable
			std::string variableName = blockVariableMapping;
			if (RefinementUtils::ContainsKey(variablesMappingsPerBlock[previousBlock], blockVariableMapping))
			{
				variableName = variablesMappingsPerBlock[previousBlock][blockVariableMapping];
			}

			std::string assignedExpr = "__value == "s + variableName;

			auto addConstRes = constraintBuilder.AddConstraintForAssignment(constraintName, mappedVariableName, assignedExpr, blockVariablesAndInfo);
			if (!addConstRes.Succeeded) { return addConstRes; }
		}

		variablesMappingsPerBlock[currentBlockName][variable] = mappedVariableName;
		variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);
		return ResultType::Success();
	}

	ResultType VariablesEnvironment::createPhiNodeInternal(
		const std::string& variable, 
		const std::string& mappedVariableName,
		const FixpointType& type, 
		const std::vector<std::string>& sourceVariableNames, 
		const std::vector<std::string>& previousBlocks)
	{
		if (sourceVariableNames.size() != previousBlocks.size())
		{
			return ResultType::Error("Expected phi node variables and associated block size to be the same"s);
		}

		//Check if any parts of the phi node are variables that are not yet declared
		for (size_t i = 0, csize = previousBlocks.size(); i < csize; i++)
		{
			auto& previousBlock = previousBlocks[i];
			auto blockVariableMapping = sourceVariableNames[i];

			//variable of a phi node hasn't been created yet
			if (!RefinementUtils::ContainsKey(variablesMappingsPerBlock[previousBlock], blockVariableMapping))
			{
				auto futureBinderRes = constraintBuilder.CreateFutureBinder(blockVariableMapping, type);
				if (!futureBinderRes.Succeeded) { return futureBinderRes; }
			}
		}

		return createPhiNodeWithoutCreatedBinders(variable, mappedVariableName, type, sourceVariableNames, previousBlocks);
	}

	ResultType VariablesEnvironment::CreatePhiNode(const std::string& variable, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks)
	{
		std::string mappedVariableName = variable;
		return createPhiNodeInternal(variable, mappedVariableName, type, sourceVariableNames, previousBlocks);
	}

	ResultType VariablesEnvironment::getBlockGuard(const std::string& blockName, std::string& blockGuard)
	{
		if (RefinementUtils::ContainsKey(cachedBlockGuards, blockName))
		{
			blockGuard = cachedBlockGuards[blockName];
			return ResultType::Success();
		}

		if (blockName == functionBlockGraph.GetStartingBlockName())
		{
			blockGuard = "true";
		}
		else
		{
			std::vector<std::string> predecessors;
			{
				auto getPredRes = functionBlockGraph.GetPreviousBlocks(blockName, predecessors);
				if (!getPredRes.Succeeded) { return getPredRes; }
			}

			std::vector<std::string> predecessorTransitionGuards;
			for (const auto& predecessor : predecessors)
			{
				std::string transitionGuardName = "__transition__"s + predecessor + "__"s + blockName;

				std::string predecessorEntryGuard;
				{
					if (predecessor == blockName)
					{
						predecessorEntryGuard = transitionGuardName;
					}
					else
					{
						auto getPredEntryRes = getBlockGuard(predecessor, predecessorEntryGuard);
						if (!getPredEntryRes.Succeeded) { return getPredEntryRes; }
					}
				}

				predecessorTransitionGuards.emplace_back("("s + predecessorEntryGuard + " && "s + transitionGuardName + ")"s);
			}

			blockGuard = "("s + RefinementUtils::StringJoin(" || ", predecessorTransitionGuards) + ")"s;
		}

		cachedBlockGuards[blockName] = blockGuard;
		return ResultType::Success();
	}

	ResultType VariablesEnvironment::addVariableToBlockAndSuccessors(const std::string& blockName, const std::string& transitionGuardName)
	{
		std::vector<std::string> successors;
		{
			auto getSuccRes = functionBlockGraph.GetAllSuccessorBlockNames(blockName, successors);
			if (!getSuccRes.Succeeded) { return getSuccRes; }
		};

		for (const auto& successor : successors)
		{
			variablesMappingsPerBlock[successor][transitionGuardName] = transitionGuardName;
			variablesValuesPerBlock[successor].emplace(transitionGuardName);
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::initializeBlockGuards()
	{
		std::vector<std::string> blockNames;
		{
			auto getBlockRes = functionBlockGraph.GetAllBlockNames(blockNames);
			if (!getBlockRes.Succeeded) { return getBlockRes; }
		}

		for (const auto& blockName : blockNames)
		{
			std::vector<std::string> successors;
			{
				auto getSuccRes = functionBlockGraph.GetSuccessorBlocks(blockName, successors);
				if (!getSuccRes.Succeeded) { return getSuccRes; }
			}

			for(const auto& successor : successors)
			{
				std::string transitionGuardName = "__transition__"s + blockName + "__"s + successor;
				{
					auto createBinderRes = constraintBuilder.CreateFutureBinder(transitionGuardName, FixpointType::GetBoolType());
					if (!createBinderRes.Succeeded) { return createBinderRes; }
				}

				{
					auto addVarRes = addVariableToBlockAndSuccessors(successor, transitionGuardName);
					if (!addVarRes.Succeeded) { return addVarRes; }
				}
			}
		}

		for (const auto& blockName : blockNames)
		{
			std::string blockGuardName = "__block__"s + blockName;
			std::string blockGuard;
			{
				auto getBlockGuardRes = getBlockGuard(blockName, blockGuard);
				if (!getBlockGuardRes.Succeeded) { return getBlockGuardRes; }
			}

			auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(blockGuardName, FixpointType::GetBoolType(), { blockGuard });
			if (!createBinderRes.Succeeded) { return createBinderRes; }

			//Don't add block guard to the variablesMapping, as it is added in the getBinders functions
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::endBlock(const std::string& blockName)
	{
		auto& phiNodeObligationsForBlock = phiNodeObligations[blockName];

		for (const auto& phiNodeObligation : phiNodeObligationsForBlock)
		{
			auto expression = "__value == "s + GetVariableName(phiNodeObligation.VariableSource);
			auto currentMapping = phiNodeObligation.TargetFutureVariable;
			auto createRes = CreateImmutableVariable(currentMapping, variableTypes.at(phiNodeObligation.VariableSource), {}, expression);
			if (!createRes.Succeeded) { return createRes; };
		}

		phiNodeObligationsForBlock.clear();

		finishedBlocks.emplace(blockName);
		return ResultType::Success();
	}

	ResultType VariablesEnvironment::StartBlock(const std::string& blockName)
	{
		if (currentBlockName == ""s)
		{
			auto initRes = initializeBlockGuards();
			if (!initRes.Succeeded) { return initRes; }
		}
		else
		{
			auto endBlockRes = endBlock(currentBlockName);
			if (!endBlockRes.Succeeded) { return endBlockRes; }
		}

		currentBlockName = blockName;

		if (RefinementUtils::Contains(finishedBlocks, currentBlockName))
		{
			return ResultType::Error("Block "s + currentBlockName + " has already finished");
		}

		std::vector<std::string> previousBlocks;
		{
			auto previousBlockRes = functionBlockGraph.GetPreviousBlocks(currentBlockName, previousBlocks);
			if (!previousBlockRes.Succeeded) { return previousBlockRes; }
		}

		//get variables which are present in ALL of the predecessor blocks
		std::set<std::string> commonVariables;
		{
			auto getCommonVarsRes = getCommonVariables(previousBlocks, commonVariables);
			if (!getCommonVarsRes.Succeeded) { return getCommonVarsRes; }
		}

		//First add into the environment any variables which are unchanged in predecessors
		std::set<std::string> phiNodeVariables;
		for (auto& commonVariable : commonVariables)
		{
			bool usingIdenticalMappings = std::all_of(previousBlocks.begin(), previousBlocks.end(), [&](std::string block) {
				return variablesMappingsPerBlock[block][commonVariable] == variablesMappingsPerBlock[previousBlocks[0]][commonVariable];
			});

			if (usingIdenticalMappings)
			{
				variablesMappingsPerBlock[currentBlockName][commonVariable] = variablesMappingsPerBlock[previousBlocks[0]][commonVariable];
				variablesValuesPerBlock[currentBlockName].emplace(variablesMappingsPerBlock[previousBlocks[0]][commonVariable]);
			}
			else
			{
				phiNodeVariables.emplace(commonVariable);
			}
		}

		//Add into the environment any variables which require phi nodes
		for (auto& phiNodeVariable : phiNodeVariables)
		{
			std::string mappedVariableName = getNextVariableName(phiNodeVariable);
			auto blockSpecificVarNames = RefinementUtils::SelectString(previousBlocks, [&](const std::string& blockName) {
				return variablesMappingsPerBlock.at(blockName).at(phiNodeVariable);
			});

			auto createPhiRes = createPhiNodeWithoutCreatedBinders(phiNodeVariable, mappedVariableName, variableTypes.at(phiNodeVariable), blockSpecificVarNames, previousBlocks);
			if (!createPhiRes.Succeeded) { return createPhiRes; }

			variablesMappingsPerBlock[currentBlockName][phiNodeVariable] = mappedVariableName;
			variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::ToStringOrFailure(std::string& output)
	{
		auto endBlockRes = endBlock(currentBlockName);
		if (!endBlockRes.Succeeded) { return endBlockRes; }

		return constraintBuilder.ToStringOrFailure(output);
	}
}