#include "llvm/Transforms/LiquidFixpointBuilder/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidFixpointBuilder/FunctionBlockGraph.h"
#include "llvm/Transforms/LiquidFixpointBuilder/ResultType.h"
#include <regex>
#include <iostream>

using namespace liquid;
using namespace std::literals::string_literals;

class LLVMFunctionBlockGraph_If : public FunctionBlockGraph
{
public:
  std::string GetStartingBlockName() const
  {
    return "entry"s;
  }

  ResultType GetSuccessorBlocks(const std::string& blockName, std::vector<std::string>& successorBlocks) const
  {
    if (blockName == "entry")
    {
      successorBlocks.emplace_back("if.then");
      successorBlocks.emplace_back("if.end");
    }
    else if (blockName == "if.then")
    {
      successorBlocks.emplace_back("if.end");
    }
    else if (blockName == "if.end")
    {

    }
    else
    {
      return ResultType::Error("NA");
    }

    return ResultType::Success();
  }

  ResultType StrictlyDominates(const std::string& firstblockName, const std::string& secondBlockName, bool& result) const
  {
    result = (firstblockName == "entry" && secondBlockName != "entry");
    return ResultType::Success();
  }
};


std::string format(VariablesEnvironment& env, std::string expression);

#define E(exp) \
{\
  ResultType a = exp; \
  if (!a.Succeeded) { str = a.ErrorMsg; goto errh; }\
}

int ifExample()
{
  LLVMFunctionBlockGraph_If llvmFunctionBlockGraph;
  VariablesEnvironment env(llvmFunctionBlockGraph);

  std::string str;

  E(env.StartBlock("entry"s));

  E(env.CreateImmutableInputVariable("a"s, FixpointType::GetIntType(), { "__value < 50" }));
  E(env.CreateMutableOutputVariable("return"s, FixpointType::GetIntType(), { "__value < 150" }));

  E(env.CreateMutableVariable("b"s, FixpointType::GetIntType(), { "__value < 100" }, format(env, "__value == {{a}} + 4"s)));
  E(env.AssignMutableVariable("b"s, format(env, "__value =={{b}} + 20"s)));
  E(env.CreateMutableVariable("cmp"s, FixpointType::GetBoolType(), {}, format(env, "__value <=> {{b}} < 60"s)));

  E(env.AddBranchInformation("cmp"s, true, "if.then"s));
  E(env.AddBranchInformation("cmp"s, false, "if.end"s));

  E(env.StartBlock("if.then"s));
  E(env.AssignMutableVariable("b"s, format(env, "__value == {{b}} + 35"s)));
  E(env.CreateImmutableVariable("c"s, FixpointType::GetIntType(), {}, format(env, "__value == 1"s)));
  E(env.AddJumpInformation("if.end"s));

  E(env.StartBlock("if.end"s));
  E(env.CreatePhiNode("d"s, FixpointType::GetIntType(), { "a"s, "c"s }, { "entry"s, "if.then"s }));

  E(env.AssignMutableVariable("return", format(env, "__value == {{b}} + {{d}}"s)));
  E(env.ToStringOrFailure(str));

  std::cout << "Succeeded:" << std::endl << str;
  return 0;

errh:
  std::cout << "Failed:" << std::endl << str;
  return -1;
}
