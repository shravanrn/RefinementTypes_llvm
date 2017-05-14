#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_VARIABLESENVIRONMENT_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_VARIABLESENVIRONMENT_H

#include <map>
#include <string>
#include <set>
#include <vector>
#include "llvm/IR/Function.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/BasicBlock.h"

namespace liquid
{
	class VariablesEnvironment
	{
	private:
		const llvm::Function& function;
		const llvm::DominatorTree& dominatorTree;
		std::map<std::string, std::set<std::string>> variables;
		std::map<std::string, std::set<std::string>> variablesInfo;
		std::map<std::string, std::set<std::string>> variablesGroupInfo;
		std::map<const llvm::Instruction*, std::string> instructionNames;

		const llvm::BasicBlock* getBlockFromFunction(const std::string& blockName);
		std::vector<std::string> getStringsInScope(const std::string blockName, std::map<std::string, std::set<std::string>>& mapping);

	public:
		VariablesEnvironment(const llvm::Function& _function, const llvm::DominatorTree& _dominatorTree) : function(_function), dominatorTree(_dominatorTree) {}
		void AddVariable(std::string blockName, std::string variable);
		void AddVariableInfo(std::string blockName, std::string infoName);
		void AddVariableInfo(std::string blockName, std::string infoGroupName, std::string infoName);
		void AddInstructionName(const llvm::Instruction& instr, const std::string& variableName);
		std::string GetInstructionName(const llvm::Instruction& instr);

		std::vector<std::string> GetVariablesInScope(const std::string blockName);
		std::vector<std::string> GetVariablesInfo(const std::string blockName);
	};
}

#endif