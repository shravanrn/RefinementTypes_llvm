#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_FUNCTIONBLOCKGRAPH_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_FUNCTIONBLOCKGRAPH_H

#include <string>
#include <vector>
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"

namespace liquid
{
	class FunctionBlockGraph
	{
	public:
		virtual std::string GetStartingBlockName() const = 0;
		virtual ResultType GetSuccessorBlocks(const std::string& blockName, std::vector<std::string>& successorBlocks) const = 0;
		virtual ResultType StrictlyDominates(const std::string& firstblockName, const std::string& secondBlockName, bool& result) const = 0;

		ResultType GetPreviousBlocks(const std::string& blockName, std::vector<std::string>& previousBlocks) const;
		ResultType GetAllBlockNames(std::vector<std::string>& blocks) const;
		ResultType GetAllSuccessorBlockNames(const std::string startingBlock, std::vector<std::string>& blocks) const;
	};
}

#endif