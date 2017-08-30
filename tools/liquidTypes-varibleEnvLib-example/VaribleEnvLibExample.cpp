#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/FunctionBlockGraph.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include <regex>
#include <iostream>

using namespace liquid;
using namespace std::literals::string_literals;

class LLVMFunctionBlockGraph : public FunctionBlockGraph
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

LLVMFunctionBlockGraph llvmFunctionBlockGraph;
VariablesEnvironment env(llvmFunctionBlockGraph);

std::string format(std::string expression)
{
  std::regex r("\\{\\{(.+?)\\}\\}");
  std::smatch m;

  while (std::regex_search(expression, m, r)) {
    std::string varname = m[1];
    std::string replacement = env.GetVariableName(varname);
    expression = m.prefix().str() + replacement + m.suffix().str();
  }

  return expression;
}

#define E(exp) \
{\
  ResultType a = exp; \
  if (!a.Succeeded) { str = a.ErrorMsg; goto errh; }\
}\

int main()
{
  std::string str;

  E(env.StartBlock("entry"s));

  E(env.CreateImmutableInputVariable("a"s, FixpointType::GetIntType(), { "__value < 50" }));
  E(env.CreateMutableOutputVariable("return"s, FixpointType::GetIntType(), { "__value < 150" }));

  E(env.CreateMutableVariable("b"s, FixpointType::GetIntType(), { "__value < 100" }, format("__value == {{a}} + 4"s)));
  E(env.AssignMutableVariable("b"s, format("__value =={{b}} + 20"s)));
  E(env.CreateMutableVariable("cmp"s, FixpointType::GetBoolType(), {}, format("__value <=> {{b}} < 60"s)));

  E(env.AddBranchInformation("cmp"s, true, "if.then"s));
  E(env.AddBranchInformation("cmp"s, false, "if.end"s));

  E(env.StartBlock("if.then"s));
  E(env.AssignMutableVariable("b"s, format("__value == {{b}} + 35"s)));
  E(env.CreateImmutableVariable("c"s, FixpointType::GetIntType(), {}, format("__value == 1"s)));
  E(env.AddJumpInformation("if.end"s));

  E(env.StartBlock("if.end"s));
  E(env.CreatePhiNode("d"s, FixpointType::GetIntType(), { "a"s, "c"s }, { "entry"s, "if.then"s }));

  E(env.AssignMutableVariable("return", format("__value == {{b}} + {{d}}"s)));
  E(env.ToStringOrFailure(str));

  std::cout << "Succeeded:" << std::endl << str;
  return 0;

errh:
  std::cout << "Failed:" << std::endl << str;
  return -1;
}
