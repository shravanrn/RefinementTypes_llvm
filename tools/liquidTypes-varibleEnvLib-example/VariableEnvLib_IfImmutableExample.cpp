#include "llvm/Transforms/LiquidFixpointBuilder/VariablesEnvironmentImmutable.h"
#include "llvm/Transforms/LiquidFixpointBuilder/FunctionBlockGraph.h"
#include "llvm/Transforms/LiquidFixpointBuilder/ResultType.h"
#include <regex>
#include <iostream>

using namespace liquid;
using namespace std::literals::string_literals;

class LLVMFunctionBlockGraph_IfImmutable: public FunctionBlockGraph
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

  ResultType StrictlyDominates(const std::string& firstBlockName, const std::string& secondBlockName, bool& result) const
  {
    result = (firstBlockName == "entry" && secondBlockName != "entry");
    return ResultType::Success();
  }
};

std::string format(VariablesEnvironmentImmutable& env, std::string expression);

#define E(exp) \
{\
 ResultType a = exp; \
 if (!a.Succeeded) { str = a.ErrorMsg; goto errh; } \
}

int ifImmutableExample()
{
  LLVMFunctionBlockGraph_IfImmutable llvmFunctionBlockGraph;
  VariablesEnvironmentImmutable env(llvmFunctionBlockGraph);

  std::string str;

  E(env.StartBlock("entry"s));

  E(env.CreateImmutableInputVariable("a"s, FixpointType::GetIntType(), { "__value == 4"s }));

  E(env.CreateImmutableVariable("b"s, FixpointType::GetIntType(), {}, format(env, " __value == 5"s)));

  E(env.CreateImmutableVariable("c"s, FixpointType::GetIntType(), {}, format(env, " __value == 6"s)));

  E(env.CreateImmutableVariable("cmp"s, FixpointType::GetBoolType(), {}, format(env, " __value <=> {{a}} == 4"s)));

  E(env.AddBranchInformation("cmp"s, true, "if.then"));
  E(env.AddBranchInformation("cmp"s, false, "if.end"));

  E(env.StartBlock("if.then"s));
  E(env.CreateImmutableVariable("d"s, FixpointType::GetIntType(), {}, format(env, "__value == {{b}}"s)));
  E(env.AddJumpInformation("if.end"s));
  
  E(env.StartBlock("if.end"s));
  E(env.CreatePhiNode("e"s, FixpointType::GetIntType(), { "c"s, "d"s }, { "entry"s, "if.then"s }));
  E(env.CreateImmutableVariable("ret"s, FixpointType::GetIntType(), {"__value == 5"}, format(env, "__value == {{e}}")));

  E(env.ToStringOrFailure(str));

  std::cout << "Succeeded:" << std::endl << str;
  return 0;

errh:
  std::cout << "Failed:" << std::endl << str;
  return -1;
}
