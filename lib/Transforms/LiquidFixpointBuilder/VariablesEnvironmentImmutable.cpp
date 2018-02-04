#include "llvm/Transforms/LiquidFixpointBuilder/VariablesEnvironmentImmutable.h"
#include "llvm/Transforms/LiquidFixpointBuilder/RefinementUtils.h"
#include <algorithm>
#include <cassert>
#include <string>

using namespace std::literals::string_literals;

namespace liquid
{
  // Check whether a variable is defined
  bool VariablesEnvironmentImmutable::IsVariableDefined(std::string variableName)
  {
    return RefinementUtils::ContainsKey(variableTypes, variableName);
  }

  // Get the address of the variable
  std::string VariablesEnvironmentImmutable::GetVariableAddress(std::string variableName)
  {
    return constraintBuilder.GetBinderAddress(variableName);
  }

  // Get the binders for the block
  std::vector<std::string> VariablesEnvironmentImmutable::getBlockBinders(const std::string& blockName)
  {
    auto currBinders = variablesValuesPerBlock[blockName];
    std::vector<std::string> ret(currBinders.begin(), currBinders.end());
    ret.emplace_back("__block__"s + blockName);
    return ret;
  }

  // Assign a Type to a pre-existing variable, or give a variable a new Type
  void insertOrAssignVarType(std::map<std::string, FixpointType>& mapToUse, const std::string& variable, const FixpointType& type)
  {
    auto search = mapToUse.find(variable);
    if (search!= mapToUse.end()) {
      mapToUse.erase(search);
    }
    mapToUse.emplace(variable, type);
  }

  // Create an Input Output Variable
  ResultType VariablesEnvironmentImmutable::createIOVariable(const std::string& variable,
							     const FixpointType& type,
							     const std::vector<std::string>& constraints)
  {
    if (RefinementUtils::ContainsKey(variableTypes, variable)) {
      return ResultType::Error("Variable"s + variable + " already exists."s);
    }

    // @TODO::(Juspreet) - Is this really necessary ? `variable` is already a const reference. 
    std::string mappedVariableName = variable;
  
    {
      auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(mappedVariableName, type, constraints);
      if (!createBinderRes.Succeeded) { return createBinderRes; }
    }

    insertOrAssignVarType(variableTypes, variable, type);
    // @TODO::(Juspreet) - Is the Environment of bindings in a block ordered ? If so, is it FIFO/LIFO ?
    variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);

    return ResultType::Success();
  }
    
  // Create an Immutable Input Variable
  ResultType VariablesEnvironmentImmutable::CreateImmutableInputVariable(
    const std::string& variable,
    const FixpointType& type,
    const std::vector<std::string>& constraints)
  {
    auto createRes = createIOVariable(variable, type, constraints);
    if (!createRes.Succeeded) { return createRes; }

    return ResultType::Success();
  }

  // Create a Variable
  ResultType VariablesEnvironmentImmutable::createVariable(
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

    // If there is any constraint, add it to the constraintBuilder and resolve it
    if (constraints.size() > 0)
    {
      std::string constraintName = "Variable_"s + mappedVariableName + "_ConstraintCheck"s;
      auto addConstRes = constraintBuilder.AddConstraint(constraintName, type, constraints, expression, currVariablesAndInfo);
      if (!addConstRes.Succeeded) { return addConstRes; }
    }

    // Make an assignment to the variable
    {
      std::string assignmentConstraintName = "Variable_"s + mappedVariableName + "_Assignment"s;
      auto addConstRes = constraintBuilder.AddConstraintForAssignment(assignmentConstraintName, mappedVariableName, expression, currVariablesAndInfo);
      if (!addConstRes.Succeeded) { return addConstRes; }
    }

    // Make updates to Variable Mappings and Values for State Update of the Block
    insertOrAssignVarType(variableTypes, variable, type);
    variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);

    return ResultType::Success();
  }
  
  // Create an Immutable Variable
  ResultType VariablesEnvironmentImmutable::CreateImmutableVariable(
    std::string variable,
    FixpointType type,
    std::vector<std::string> constraints,
    std::string expression)
  {
    if (RefinementUtils::ContainsKey(variableTypes, variable))
    {
      return ResultType::Error("Variable"s + variable + " already exists."s);
    }

    return createVariable(variable, variable, type, constraints, expression);
  }

  ResultType VariablesEnvironmentImmutable::AddJumpInformation(const std::string& targetBlock)
  {
    std::string transitionGuardName = "__transition__"s + currentBlockName + "__"s + targetBlock;
    {
      auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(transitionGuardName, FixpointType::GetBoolType(), { "true"s });
      if (!createBinderRes.Succeeded) { return createBinderRes; }
    }

    return ResultType::Success();
  }

  ResultType VariablesEnvironmentImmutable::AddBranchInformation(const std::string& booleanVariable, const bool variableValue, const std::string& targetBlock)
  {
    // Verify the variable doesn't already exist.
    if (!RefinementUtils::ContainsKey(variableTypes, booleanVariable))
    {
      return ResultType::Error("Missing variable: "s + booleanVariable);
    }

    // Enforce the variable type is Boolean.
    if (variableTypes.at(booleanVariable) != FixpointType::GetBoolType())
    {
      return ResultType::Error("Expected boolean type for variable: "s + booleanVariable);
    }

    // Convert the variable into a Proposition for a Fixpoint Constraint.
    const std::string assignedExpr = "__value <=> "s + (variableValue ? ""s : "~"s) + booleanVariable;
    const std::string transitionGuardName = "__transition__"s + currentBlockName + "__"s + targetBlock;
    
    {
      auto createdBinderRes = constraintBuilder.CreateBinderWithConstraints(transitionGuardName, FixpointType::GetBoolType(),  { assignedExpr });
      if (!createdBinderRes.Succeeded) { return createdBinderRes; }
    }

    return ResultType::Success();
  }
  
  ResultType VariablesEnvironmentImmutable::getCommonVariables(std::vector<std::string> previousBlocks, std::set<std::string>& commonVariables)
  {
    std::vector<std::string> previousFinishedBlocks;
    std::vector<std::string> previousUnfinishedBlocks;

    // Iterate through previousBlocks, and build the finished and unfinished blocks.
    for (auto& previousBlock: previousBlocks)
    {
      RefinementUtils::Contains(finishedBlocks, previousBlock) ?
	previousFinishedBlocks.push_back(previousBlock) :
	previousUnfinishedBlocks.push_back(previousBlock);
    }

    // In a CFG, it is possible that previous blocks aren't _all_ processed (loops).
    // However, for immutable constructs, this shouldn't happen. Raise an Error, if it does.
    if (previousUnfinishedBlocks.size() > 0)
    {
      return ResultType::Error("There exist: "s + std::to_string(previousUnfinishedBlocks.size()) + " unfinished Block/s that were appended without processing. This is an error in an immutable environment."s);
    }

    bool first = true;
    bool allDominated = true;

    for (int i = 0; i < previousFinishedBlocks.size(); i++)
    {
      auto& currFinishedBlock = previousFinishedBlocks[i];

      // Verify that the block and its values are present in the known blocks.
      if (!RefinementUtils::Contains(variablesValuesPerBlock, currFinishedBlock)) {
	return ResultType::Error("The Block: "s + std::to_string(currFinishedBlock) + " violates the processing order."s);
      }

      if (first)
      {
	commonVariables = RefinementUtils::GetValuesSet(variablesValuesPerBlock[currFinishedBlock]);
	first = false;
      } else {
	std::set<std::string> currBlockVars = RefinementUtils::GetValuesSet(variablesValuesPerBlock[currFinishedBlock]);

	// @TODO::(Juspreet) - This is wrong. This will not work with 'for' loops. Fix as soon as a strategy for Fixpoint is developed.
	for (int j = 0; j < i; j++)
	{
	  auto& previousFinishedBlock = previousFinishedBlocks[j];

	  bool dominated;
	  {
	    auto domRes = functionBlockGraph.StrictlyDominates(previousFinishedBlock, currFinishedBlock, dominated);
	    if (!domRes.Succeeded) { return domRes; }
	  }

	  if (!dominated) {
	    allDominated = false;
	    break;
	  }
	  
	}

	// If all blocks are not dominated, then exit with an appropriate error message.
	if (!allDominated)
	  {
	    return ResultType::Error("Blocks were not completed in the right order. Unexpected control flow graph. Finished: "s
				     + RefinementUtils::StringJoin(", "s, previousFinishedBlocks)
				     + " . Unfinished: "s
				     + RefinementUtils::StringJoin(", "s, previousUnfinishedBlocks)
				     );
	  }
	
	commonVariables = RefinementUtils::SetIntersection(commonVariables, currBlockVars);
      }
    }

    return ResultType::Success();
  }

  ResultType VariablesEnvironmentImmutable::getBlockBindersForPhiNode(const std::string& previousBlock, const std::string& blockName, const std::string& mappedVariableName, std::vector<std::string>& binders)
  {
    binders = getBlockBinders(previousBlock);

    // Add the transition Edge Type to the Environment
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
	// NOTE: (Juspreet) - In immutable environments, the "mapping" is static. As such, the name will _always_ be used as is.
	std::string variableName = blockVariableMapping;

	std::string assignedExpr = "__value == "s + variableName;
	
	auto addConstRes = constraintBuilder.AddConstraintForAssignment(constraintName, mappedVariableName, assignedExpr, blockVariablesAndInfo);
	if (!addConstRes.Succeeded) { return addConstRes; }
      }
    
    variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);
    return ResultType::Success();
  }

  ResultType VariablesEnvironmentImmutable::createPhiNodeInternal(
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

    // Check if any parts of the phi node are variables that are not yet declared.
    for (size_t i = 0, csize = previousBlocks.size(); i < csize; i++)
    {
      auto& previousBlock = previousBlocks[i];
      auto blockVariableMapping = sourceVariableNames[i];

      // Variable of a phi node hasn't been created yet.
      if (!RefinementUtils::ContainsKey(variablesValuesPerBlock[previousBlock], blockVariableMapping))
	{
	  auto futureBinderRes = constraintBuilder.CreateFutureBinder(blockVariableMapping, type);
	  if (!futureBinderRes.Succeeded) { return futureBinderRes; }
	}
    }
    
    return createPhiNodeWithoutCreatedBinders(variable, mappedVariableName, type, sourceVariableNames, previousBlocks);
  }

  ResultType VariablesEnvironmentImmutable::CreatePhiNode(const std::string& variable, const FixpointType& type, const std::vector<std::string>& sourceVariableNames, const std::vector<std::string>& previousBlocks)
  {
    std::string mappedVariableName = variable;
    return createPhiNodeInternal(variable, mappedVariableName, type, sourceVariableNames, previousBlocks);
  }

  //@TODO::(Juspreet) - Replace with Fixpoint Algorithm. To come in Part-3.
  ResultType VariablesEnvironmentImmutable::getBlockGuard(const std::string& blockName, std::string& blockGuard)
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

	// (Bug?) The recursive call later gives unallocated strings here, until we find a predecessor that is the same as the current block.
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

  ResultType VariablesEnvironmentImmutable::addVariableToBlockAndSuccessors(const std::string& blockName, const std::string& transitionGuardName)
  {
    std::vector<std::string> successors;
    {
      auto getSuccRes = functionBlockGraph.GetAllSuccessorBlockNames(blockName, successors);
      if (!getSuccRes.Succeeded) { return getSuccRes; }
    };
    
    for (const auto& successor : successors)
      {
	variablesValuesPerBlock[successor].emplace(transitionGuardName);
      }
    
    return ResultType::Success();
  }
  
  ResultType VariablesEnvironmentImmutable::initializeBlockGuards()
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

  ResultType VariablesEnvironmentImmutable::endBlock(const std::string& blockName)
  {
    finishedBlocks.emplace(blockName);
    return ResultType::Success();
  }

  ResultType VariablesEnvironmentImmutable::StartBlock(const std::string& blockName)
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
    
    // Get variables which are present in ALL of the predecessor blocks
    std::set<std::string> commonVariables;
    {
      auto getCommonVarsRes = getCommonVariables(previousBlocks, commonVariables);
      if (!getCommonVarsRes.Succeeded) { return getCommonVarsRes; }
    }
    
    // Add to the environment any variables which are unchanged in predecessors
    // This block of code is hazy at best --> We may need to revisit it during for-loops. However, for now:
    // *NO* variable that is assigned to can be changed. This is enforced in immutability (with the exception of for-loops).
    std::set<std::string> phiNodeVariables;
    for (auto& commonVariable : commonVariables)
    {
      bool usingIdenticalMappings = true;

      // Extract the variable values in the most current previous block as a set.
      std::set<string> recentPreviousBlockValueSet = RefinementUtils::GetValuesSet(variablesValuesPerBlock[previousBlocks[0]]);

      for (auto& currPreviousBlock: previousBlocks)
      {
	// Extract the variable values in the current previous block. 
	std::set<string> currPreviousBlockValueSet = RefinementUtils::GetValuesSet(variablesValuesPerBlock[currPreviousBlock]);

	// Compare equality and conjunct with the condition.
	// Questions:
	// i) It seems this check is useless. This can only be false if a variable is deleted. That seems impossible in the immutable case.
	usingIdenticalMappings = usingIdenticalMappings && (currPreviousBlockValueSet[commonVariable] == recentPreviousBlockValueSet[commonVariable]);
      }
      
      if (usingIdenticalMappings)
      {
	// Append the common variable from the predecessors to the current block.
	// (NOTE): This will (again), break when we are handling for-loops (with the current way variablesValuesPerBlock is used).
	variablesValuesPerBlock[currentBlockName].emplace(commonVariable);
      }
      else
      {
	return ResultType::Error("phiNodeVariables was not empty while verifying immutability of Common Variables. The variable: " + commonVariable + " was found mutated!";
      }
    }

    // Add to the environment any variables which require phi nodes
    // Juspreet: For now, this code is commented, as it should not be required.
    //for (auto& phiNodeVariable : phiNodeVariables)
    //{
      // Initialize the counter for the PhiNodevariable to be 0. Immutability => No increment.
      //std::string mappedVariableName = phiNodeVariable + "__"s + std::to_string(0);
      //auto blockSpecificVarNames = RefinementUtils::SelectString(previousBlocks, [&](const std::string& blockName) {
      //  return variablesMappingsPerBlock.at(blockName).at(phiNodeVariable);
      //});

      //auto createPhiRes = createPhiNodeWithoutCreatedBinders(phiNodeVariable, mappedVariableName, variableTypes.at(phiNodeVariable), blockSpecificVarNames, previousBlocks);
      //if (!createPhiRes.Succeeded) { return createPhiRes; }
      
      //variablesValuesPerBlock[currentBlockName].emplace(mappedVariableName);
      //}
    
    return ResultType::Success();
  }
  
  ResultType VariablesEnvironmentImmutable::ToStringOrFailure(std::string& output)
  {
    auto endBlockRes = endBlock(currentBlockName);
    if (!endBlockRes.Succeeded) { return endBlockRes; }
    
    return constraintBuilder.ToStringOrFailure(output);
  }
}
