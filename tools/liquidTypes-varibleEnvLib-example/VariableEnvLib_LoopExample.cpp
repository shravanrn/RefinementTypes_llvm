#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/FunctionBlockGraph.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include <regex>
#include <iostream>

using namespace liquid;
using namespace std::literals::string_literals;

class LLVMFunctionBlockGraph_ForLoop: public FunctionBlockGraph
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
      successorBlocks.emplace_back("condition");
      successorBlocks.emplace_back("if.then");
      successorBlocks.emplace_back("if.end");
    }
    else if (blockName == "condition")
    {
      successorBlocks.emplace_back("if.then");
      successorBlocks.emplace_back("if.end");
    }
    else if (blockName == "if.then")
    {
      successorBlocks.emplace_back("condition");
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
    result = (firstBlockName == "entry" && secondBlockName != "entry") || (firstBlockName == "condition" && (secondBlockName != "condition" && secondBlockName != "entry"));
    return ResultType::Success();
  }
};

std::string format(VariablesEnvironment& env, std::string expression);

#define E(exp) \
{\
  ResultType a = exp; \
  if (!a.Succeeded) { str = a.ErrorMsg; goto errh; } \
}

int forLoopExample()
{
  /* Code to Verify :
     int i;
     int j;
     for (int i = 0; i < 20; i++)
     {
     }
     j = i;
  */
  LLVMFunctionBlockGraph_ForLoop llvmFunctionBlockGraph;
  VariablesEnvironment env(llvmFunctionBlockGraph);

  std::string str;

  /* Corresponding 3-address code :
     int i;
     int j;
     int i' = 0;
     if (i' < 20) goto {{ i' = i' + 1 }}
     j = i;
  */

  /* Basic Blocks :
     B1 = [int i; .. int i' = 0]
     B2 = [if (i' < 20) goto {{ B4 }}]
     B3 = [j = i]
     B4 = [i' = i' + 1; goto {{ B2 }}]
  */

  // Create Start Block
  E(env.StartBlock("entry"s));

  // Create mutable `i`
  E(env.CreateMutableVariable("i"s, FixpointType::GetIntType(), {}, format(env, "__value <= 2147483647"s )));

  // Create mutable variable `return`
  E(env.CreateMutableVariable("return"s, FixpointType::GetIntType(), {}, format(env, "__value == 20"s )));

  // Create mutable variable `i_one`
  E(env.CreateMutableVariable("i_one"s, FixpointType::GetIntType(), {}, format(env, "__value == 0"s )));

  // Create the "condition" block
  E(env.StartBlock("condition"s));
  E(env.CreateMutableVariable("cmp"s, FixpointType::GetBoolType(), {}, format(env, "__value <=> {{i_one}} < 20"s)));

  // Add Branching information
  E(env.AddBranchInformation("cmp"s, true, "if.then"s));
  E(env.AddBranchInformation("cmp"s, false, "if.end"s));
  
  // Create the "if.then" block
  E(env.StartBlock("if.then"s));
  E(env.AssignMutableVariable("i_one"s, format(env, "__value == {{i_one}} + 1"s)));
  E(env.AddJumpInformation("condition"s));

  // Create the "if.end" block
  E(env.StartBlock("if.end"s));
  E(env.AssignMutableVariable("return"s, format(env, "__value == {i}"s)));

  // Check for failure, and verify this works; or fail and be loud
  E(env.ToStringOrFailure(str));

  std::cout << "Succeeded:" << std::endl << str;
  return 0;

errh:
  std::cout << "Failed:" << std::endl << str;
  return -1;
}
