#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"

namespace liquid
{
	const llvm::BasicBlock* VariablesEnvironment::getBlockFromFunction(const std::string& blockName)
	{
		for (auto& blockPtr : function)
		{
			if (blockPtr.getName().str() == blockName)
			{
				return &blockPtr;
			}
		}

		assert(false);
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

	void VariablesEnvironment::AddVariableInfo(std::string blockName, std::string infoGroupName, std::string infoName)
	{
		if (RefinementUtils::containsKey(variablesGroupInfo, infoGroupName))
		{
			auto& existingGroupInfos = variablesGroupInfo[infoGroupName];
			for (auto& existingInfo : existingGroupInfos)
			{
				variablesInfo[blockName].erase(existingInfo);
			}

			existingGroupInfos.clear();
		}

		variablesGroupInfo[infoGroupName].emplace(infoName);
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

	void VariablesEnvironment::AddInstructionName(const llvm::Instruction& instr, const std::string& variableName)
	{
		instructionNames[&instr] = variableName;
	}

	std::string VariablesEnvironment::GetInstructionName(const llvm::Instruction& instr)
	{
		assert(RefinementUtils::containsKey(instructionNames, &instr));
		return instructionNames[&instr];
	}
}