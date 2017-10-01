#include "llvm/Transforms/LiquidFixpointBuilder/FunctionBlockGraph.h"

namespace liquid
{
	ResultType FunctionBlockGraph::GetPreviousBlocks(const std::string& blockName, std::vector<std::string>& previousBlocks) const
	{
		std::string entryBlockName = GetStartingBlockName();
		std::vector<std::string> queue{ entryBlockName };
		std::set<std::string> alreadySeen{ entryBlockName };

		while (queue.size() != 0)
		{
			auto curr = queue[0];
			queue.erase(queue.begin());

			std::vector<std::string> successorBlocks;
			{
				auto successorRes = GetSuccessorBlocks(curr, successorBlocks);
				if (!successorRes.Succeeded) { return successorRes; }
			}

			if (RefinementUtils::Contains(successorBlocks, blockName))
			{
				previousBlocks.emplace_back(curr);
			}

			for (const auto& currSuccessor : successorBlocks)
			{
				if (!RefinementUtils::Contains(alreadySeen, currSuccessor))
				{
					alreadySeen.emplace(currSuccessor);
					queue.push_back(currSuccessor);
				}
			}
		}

		return ResultType::Success();
	}

	ResultType FunctionBlockGraph::GetAllSuccessorBlockNames(const std::string startingBlock, std::vector<std::string>& blocks) const
	{
		std::vector<std::string> queue{ startingBlock };		
		blocks.emplace_back(startingBlock);

		while (queue.size() != 0)
		{
			auto curr = queue[0];
			queue.erase(queue.begin());

			std::vector<std::string> successorBlocks;
			{
				auto successorRes = GetSuccessorBlocks(curr, successorBlocks);
				if (!successorRes.Succeeded) { return successorRes; }
			}

			for (const auto& currSuccessor : successorBlocks)
			{
				if (!RefinementUtils::Contains(blocks, currSuccessor))
				{
					blocks.emplace_back(currSuccessor);
					queue.push_back(currSuccessor);
				}
			}
		}

		return ResultType::Success();
	}

	ResultType FunctionBlockGraph::GetAllBlockNames(std::vector<std::string>& blocks) const
	{
		std::string entryBlockName = GetStartingBlockName();
		return GetAllSuccessorBlockNames(entryBlockName, blocks);
	}
}
