#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include <algorithm>
#include <cassert>

using namespace std::literals::string_literals;

namespace liquid
{
	bool VariablesEnvironment::IsVariableDefined(std::string variableName)
	{
		return RefinementUtils::containsKey(variableTypes, variableName);
	}

	std::string VariablesEnvironment::getNextVariableName(const std::string& variable)
	{
		std::string newVarName = variable + "__"s + mutableVariableState[variable].GetNextIdString();
		return newVarName;
	}

	std::string VariablesEnvironment::GetVariableName(std::string variableName)
	{
		assert(RefinementUtils::containsKey(variablesMappingsPerBlock, currentBlockName));
		assert(RefinementUtils::containsKey(variablesMappingsPerBlock[currentBlockName], variableName));
		return variablesMappingsPerBlock[currentBlockName][variableName];
	}

	ResultType VariablesEnvironment::addInformation(const std::string& blockName, const std::string& variableName, bool information)
	{
		if (RefinementUtils::containsKey(booleanInformation[blockName], variableName))
		{
			if (booleanInformation[blockName][variableName] != information)
			{
				return ResultType::Error("Added contrary information for variable: "s + variableName + " in block: "s + blockName);
			}

			return ResultType::Success();
		}

		if (!RefinementUtils::containsKey(createdInfoBinders[variableName], information))
		{
			std::string currentVarMapping = GetVariableName(variableName);
			std::string varUniqueConstraintName = getNextVariableName(variableName);
			std::string qualifier = information ? "__value"s : "~__value"s;

			{
				auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(varUniqueConstraintName, currentVarMapping, variableTypes.at(variableName), { qualifier });
				if (!createBinderRes.Succeeded) { return createBinderRes; }
			}

			createdInfoBinders[variableName][information] = varUniqueConstraintName;
		}

		booleanInformation[blockName][variableName] = information;
		return ResultType::Success();
	}

	ResultType VariablesEnvironment::getInformationBinders(const std::string& blockName, std::vector<std::string>& informationBinders)
	{
		std::vector<std::string> previousBlocks;
		{
			auto prevBlocksRes = functionBlockGraph.GetPreviousBlocks(blockName, previousBlocks);
			if (!prevBlocksRes.Succeeded) { return prevBlocksRes; }
		}

		if (previousBlocks.size() > 0)
		{
			std::set<std::string> variableNames = RefinementUtils::GetKeysSet(booleanInformation[previousBlocks[0]]);
			for (unsigned i = 1; i < variableNames.size(); i++)
			{
				auto currBlockVars = RefinementUtils::GetKeysSet(booleanInformation[previousBlocks[i]]);
				std::set<std::string> intersectedVariables;
				std::set_intersection(variableNames.begin(), variableNames.end(), currBlockVars.begin(), currBlockVars.end(), std::inserter(intersectedVariables, intersectedVariables.begin()));
				variableNames = intersectedVariables;
			}

			for (const auto& variableName : variableNames)
			{
				bool allHaveSameValue = std::all_of(previousBlocks.begin(), previousBlocks.end(), [&](std::string block) {
					return booleanInformation[block][variableName] == booleanInformation[previousBlocks[0]][variableName];
				});

				if (allHaveSameValue)
				{
					auto variableVal = booleanInformation[previousBlocks[0]][variableName];
					if (RefinementUtils::containsKey(booleanInformation[blockName], variableName))
					{
						if (booleanInformation[blockName][variableName] != variableVal)
						{
							return ResultType::Error("Added contrary information for variable: "s + variableName + " in block: "s + blockName);
						}
					}
					informationBinders.emplace_back(createdInfoBinders[variableName][variableVal]);
				}
			}
		}

		auto& currentBlockInfo = booleanInformation[blockName];
		for (const auto& varInfo : currentBlockInfo)
		{
			const auto& varName = varInfo.first;
			auto info = varInfo.second;
			informationBinders.emplace_back(createdInfoBinders[varName][info]);
		}

		return ResultType::Success();
	}

	std::vector<std::string> VariablesEnvironment::getBlockBinders(const std::string& blockName)
	{
		auto currBinders = variablesValuesPerBlock[blockName];
		std::vector<std::string> ret(currBinders.begin(), currBinders.end());
		getInformationBinders(blockName, ret);
		return ret;
	}

	ResultType VariablesEnvironment::createIOVariable(
		const std::string& variable,
		const FixpointType& type,
		const std::vector<std::string>& constraints)
	{
		if (RefinementUtils::containsKey(variableTypes, variable)) { return ResultType::Error("Variable"s + variable + " already exists."s); }

		std::string mappedVariableName = variable;

		{
			auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(mappedVariableName, type, constraints);
			if (!createBinderRes.Succeeded) { return createBinderRes; }
		}

		variableTypes.insert_or_assign(variable, type);
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
			if (!RefinementUtils::containsKey(blockVarMap.second, variable))
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

		variableTypes.insert_or_assign(variable, type);
		variablesMappingsPerBlock[currentBlockName][variable] = mappedVariableName;
		variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);
		return ResultType::Success();
	}

	ResultType VariablesEnvironment::CreateImmutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression)
	{
		if (RefinementUtils::containsKey(variableTypes, variable)) { return ResultType::Error("Variable"s + variable + " already exists."s); }
		return createVariable(variable, variable, type, constraints, expression);
	}

	ResultType VariablesEnvironment::CreateMutableVariable(std::string variable, FixpointType type, std::vector<std::string> constraints, std::string expression)
	{
		if (RefinementUtils::containsKey(variableTypes, variable)) { return ResultType::Error("Variable"s + variable + " already exists."s); }
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
		return createVariable(variable, newVarName, variableTypes.at(variable), mutableVariableConstraints[variable], expression);
	}

	//We now have to check if we can add branch information to the target. This can happen only in certain cases.
	//Consider 4 basic structures
	//
	// 1)
	// if(cond) {
	//     condSuccessCode();
	// }
	// afterIfCode();
	//
	// 2)
	// if(cond) {
	//     condSuccessCode();
	// } else {
	//     condFailCode();
	// }
	// afterIfCode();
	//
	// 3)
	// do {
	//     loopCode();
	// } while(cond);
	// afterLoopCode();
	//
	// 4)
	// while(cond) {
	//     loopCode();
	// };
	//
	// afterLoopCode();
	//
	//In each case the currentBlock, is the block with the branch. 
	//	For instance the block before the 'if' keyword for 1 and 2, and the block before the 'while' keyword for 3 and 4
	//Looking at these 4 structures, we should consider when we can "cond=true" or "cond=false" to the various blocks
	//Note: In every case, we are adding info, there is also a block which we are trying to add the contrary information to aka the contrary block
	//	For example in structure 1, when we are trying to add "cond=true", to condSuccessCode, the contrary block is afterIfCode
	//		In structure 1, when we are trying to add "cond=false", to afterIfCode, the contrary block is condSuccessCode
	//	Similarly for structures 2,3,4
	//This gives us the following rules
	//if currentBlock doesn't dominate the targetBlock, don't add the information
	//if there is a path from the contraryBlock to the targetBlock, don't add the information
	//else add the information
	ResultType VariablesEnvironment::AddBranchInformation(const std::string& booleanVariable, const bool variableValue, const std::string& targetBlock, const std::string& contraryBlock)
	{
		if (!RefinementUtils::containsKey(variableTypes, booleanVariable))
		{
			return ResultType::Error("Missing variable: "s + booleanVariable);
		}

		if (variableTypes.at(booleanVariable) != FixpointType::GetBoolType())
		{
			return ResultType::Error("Expected boolean type for variable: "s + booleanVariable);
		}

		bool strictlyDominates;
		{
			auto domRes = functionBlockGraph.StrictlyDominates(currentBlockName, targetBlock, strictlyDominates);
			if (!domRes.Succeeded) { return domRes; }
		}

		if (!strictlyDominates)
		{
			return ResultType::Success();
		}

		bool pathExists;
		{
			auto findPathRes = functionBlockGraph.PathBetweenBlocksExistsExcludingBlock(contraryBlock, targetBlock, currentBlockName, pathExists);
			if (!findPathRes.Succeeded) { return findPathRes; }
		}

		if (pathExists)
		{
			return ResultType::Success();
		}

		{
			auto addInfoRes = addInformation(targetBlock, booleanVariable, variableValue);
			if (!addInfoRes.Succeeded) { return addInfoRes; }
		}

		return ResultType::Success();
	}

	ResultType VariablesEnvironment::validateFinishedAndUnfinishedBlockStates(const std::vector<std::string>& previousFinishedBlocks, const std::vector<std::string>& previousUnfinishedBlocks)
	{
		if (previousFinishedBlocks.size() == 0)
		{
			return ResultType::Error("Blocks were not completed in the right order. Expected completion of at least one of block "s + RefinementUtils::stringJoin(", "s, previousUnfinishedBlocks));
		}
		else if (previousFinishedBlocks.size() > 1)
		{
			return ResultType::Error("Blocks were not completed in the right order. Multiple completed blocks were not expected "s + RefinementUtils::stringJoin(", "s, previousFinishedBlocks));
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
					+ RefinementUtils::stringJoin(", "s, previousFinishedBlocks)
					+ " . Unfinished: "s
					+ RefinementUtils::stringJoin(", "s, previousUnfinishedBlocks)
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

				auto allBlocks = RefinementUtils::vectorAppend({ previousFinishedBlock }, previousUnfinishedBlocks);
				auto allPhiVars = RefinementUtils::vectorAppend({ finishedBlockVariable }, phiVariableNames);

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
			auto blockVariablesAndInfo = getBlockBinders(previousBlock);
			auto blockVariableMapping = sourceVariableNames[i];
			//ensure that the future binder is part of the info
			if (!RefinementUtils::Contains(blockVariablesAndInfo, blockVariableMapping))
			{
				blockVariablesAndInfo.push_back(blockVariableMapping);
			}

			std::string constraintName = "Variable_"s + previousBlock + "_"s + currentBlockName + "_"s + variable;

			//hack - for variables that aren't created yet aka future binders, there will be no mapping, so we will just use the name as is
			//we are relying on the fact, that the first variable mapping is the same as the variable name and that future binders are immutable
			std::string variableName = blockVariableMapping;
			if (RefinementUtils::containsKey(variablesMappingsPerBlock[previousBlock], blockVariableMapping))
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
			if (!RefinementUtils::containsKey(variablesMappingsPerBlock[previousBlock], blockVariableMapping))
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
		if (currentBlockName != ""s)
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
			auto blockSpecificVarNames = RefinementUtils::selectString(previousBlocks, [&](const std::string& blockName) {
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