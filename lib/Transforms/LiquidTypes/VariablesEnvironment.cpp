#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"

const llvm::BasicBlock* VariablesEnvironment::getBlockFromFunction(const std::string& blockName)
{
	for (auto& blockPtr : function)
	{
		if (blockPtr.getName().str() == blockName)
		{
			return &blockPtr;
		}
	}

	return nullptr;
}

std::vector<std::string> VariablesEnvironment::getStringsInScope(const std::string blockName, std::map<std::string, std::set<std::string>>& mapping)
{
	std::vector<std::string> ret;
	auto blockRef = getBlockFromFunction(blockName);

	for (auto& blockVarsMapping : mapping)
	{
		auto& currBlock = blockVarsMapping.first;
		auto& currBlockVars = blockVarsMapping.second;

		auto currBlockRef = getBlockFromFunction(currBlock);

		if (dominatorTree.dominates(currBlockRef, blockRef))
		{
			ret.insert(ret.end(), currBlockVars.begin(), currBlockVars.end());
		}
	}

	return ret;
}

void VariablesEnvironment::AddVariable(std::string blockName, std::string variable) 
{ 
	variables[blockName].emplace(variable); 
}

void VariablesEnvironment::AddVariableInfo(std::string blockName, std::string infoName) 
{ 
	variablesInfo[blockName].emplace(infoName); 
}

std::vector<std::string> VariablesEnvironment::GetVariablesInScope(const std::string blockName)
{
	auto ret = getStringsInScope(blockName, variables);
	return ret;
}

std::vector<std::string> VariablesEnvironment::GetVariablesInfo(const std::string blockName)
{
	auto ret = getStringsInScope(blockName, variablesInfo);
	return ret;
}