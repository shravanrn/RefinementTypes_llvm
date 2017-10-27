#include "llvm/Transforms/LiquidFixpointBuilder/VariablesEnvironmentImmutable.h"
#include "llvm/Transforms/LiquidFixpointBuilder/RefinementUtils.h"
#include <algorithm>
#include <cassert>

using namespace std::literals::string_literals;

namepsace liquid
{
  // Check whether a variable is defined
  bool VariablesEnvironmentImmutable::IsVariableDefined(std::string variableName)
  {
    return RefinementUtils::ContainsKey(variableTypes, variableName);
  }
  
  // Get the name of the variable
  std::string VariablesEnvironmentImmutable::GetVariableName(const std::string variableName)
  {
    assert(RefinementUtils::ContainsKey(variablesMappingsPerBlock, currentBlockName));
    assert(RefinementUtils::ContainsKey(variablesMappingsPerBlock[currentBlockName], variableName));
    return variablesMappingPerBlock[currentBlockName][variableName];
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
  void insertOrAssignType(std::map<std::string, FixpointType>& mapToUse, const std::string& variable, const FixpointType& type)
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

    std::string mappedVariableName = variable;
  
    {
      auto createBinderRes = constraintBuilder.CreateBinderWithConstraints(mappedVariableName, type, constraints);
      if (!createBinderRes.Succeeded) { return createBinderRes; }
    }

    insertOrAssignType(variableTypes, variable, type);
    variablesMappingsPerBlock[currentBlockName][variable] = mappedVariableName;
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

	// Since, this is an input variable make it accessible to every block
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

  // Create a Variable
  ResultType VariablesEnvironmentImmutable::CreateVariable(
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
      std::string constraintName = "Variable_"s + mappedVariableName + "_Assignment"s;
      auto addConstRes = constraintBuilder.AddConstraintForAssignment(constaintName, mappedVariableName, expression, currVariablesAndInfo);
      if (!addConstRes.Succeeded) { return addConstRes; }
    }

    // Make updates to Variable Mappings and Values for State Update of the Block
    insertOrAssignVarType(variableTypes, variable, type);
    variablesMappingsPerBlock[currentBlockName][variable] = mappedVariableName;
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
    if (RefinementUtils::ConstainsKey(variableTypes, variable))
    {
      return ResultType::Error("Variable"s + variable + " already exists."s);
    }

    return CreateVariable(variable, variable, type, constraints, expression);
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

  ResultType VariablesEnvironment::AddBranchinformation(const std::string& booleanVariable, const bool variableValue, const std::string& targetBlock)
  {
    // Verify the variable doesn't already exist
    if (!RefinementUtils::ContainsKey(variableTypes, booleanVariable))
    {
      return ResultType::Error("Missing variable: "s + booleanVariable);
    }

    // Enforce the variable type is Boolean
    if (variableTypes.at(booleanVariable) != FixpointType::GetBoolType())
    {
      return ResultType::Error("Expected boolean type for variable: "s + booleanVariable);
    }

    // Convert the variable into a Proposition for a Fixpoint Constraint
    const std::string assignedExpr = "__value <=> "s + (variableValue ? ""s : "~"s) + booleanVariable;
    const std::string transitionGuardName = "__transition__"s + currentBlockName + "__"s + targetBlock;
    
    {
      auto createbinderRes = constraintBuilder.CreateBinderWithConstraints(transitionGuardName, FixpointType::GetBoolType(),  { assignedExpr });
      if (!createdBinderRes.Succeeded) { return createBinderRes; }
    }

    return ResultType::Success();
  }

  ResultType VariablesEnvironmentImmutable::validateFinishedAndUnfinishedBlockStates(
    const std::vector<std::string>& previousFinishedBlocks,
    const std::vector<std::string>& previousUnfinishedBlocks)
  {
    //@TODO::(juspreet) - The logic is odd. It might be the cause of problem in loops. Verify.
    if (previousFinishedBlocks.size() == 0 || previousFinishedBlocks.size() > 1) {
      return ResultType::Error("Blocks were not completed in the right order. Expected one Completed Block, got "s + std::to_string(previousFinishedBlocks.size()));
    }

    // Ensure that :
    // 1) forall blocks in PreviousFinishedBlocks, dominate(blocks, block), forall block in PreviousUnfinishedblocks
    for (const auto& previousUnfinishedBlock : previousFinishedBlocks)
    {
      bool allDominated = true;
      for (const auto& previousFinishedBlock : previousUnfinishedBlocks)
      {
	bool dominated;
	{
	  auto domRes = functionBlockGraph.StrictlyDominates(previousFinishedBlock, previousUnfinishedBlock, dominate);
	  if (!domRes.Succeeded) { return domRes; }
	}

	if (!dominated)
	{
	  allDominated = false;
	  break;
	}
      }

      // If all blocks are not dominated, then exit with an appropriate error message
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
  
  ResultType VariablesEnvironmentImmutable::getCommonVariables(std::vector<std::string> previousBlocks, std::set<std::string>& commonVariables)
  {
    std::vector<std::string> previousFinishedBlocks;
    std::vector<std::string> previousUnfinishedBlocks;

    for (auto& previousBlock: previousBlocks)
    {
      RefinementUtils::Contains(finishedBlocks, previousBlock) ?
	previousFinishedBlocks.push_back(previousBlock) :
	previousUnfinishedBlocks.push_back(previousBlock);
    }

    // In a CFG, it is possible that previous blocks aren't _all_ processed (loops)
    // However, for immutable constructs, this shouldn't happen. Raise an Error, if it does.
    if (previousUnfinishedBlocks.size() > 0)
    {
      return ResultType::Error("There exist: "s + std::to_string(previousUnfinishedBlocks.size()) + " unfinished Block that were appended without processing. This is an error in an immutable environment."s);
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

  //@TODO::juspreet - Continue ripping and verifying.
  
  
}
