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
 
  // Create an Immutable Input Variable
  ResultType VariablesEnvironmentImmutable: :CreateImmutableInputVariable(
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

  
  
  
