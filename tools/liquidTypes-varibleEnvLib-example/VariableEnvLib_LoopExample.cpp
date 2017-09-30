#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/FunctionBlockGraph.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include <regex>
#include <iostream>

using namespace liquid;
using namespace std::literals::string_literals;

class LLVMFunctionBlockGraph_Loop: public FunctionBlockGraph
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
      successorBlocks.emplace_back("if.loop");
    }
    else if (blockName == "if.loop")
    {
      successorBlocks.emplace_back("if.loop");
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
    result = ((firstBlockName == "entry" && secondBlockName != "entry") || (firstBlockName == "if.loop" && (secondBlockName != "if.loop" && secondBlockName != "entry")));
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
  LLVMFunctionBlockGraph_Loop llvmFunctionBlockGraph;
  VariablesEnvironment env(llvmFunctionBlockGraph);

  std::string str;

  /* Corresponding 3-address code :
     int i;
     int j {__value = 5 __};
     int i' = 0;
     if (i' < 20) goto {{ i' = i' + 1 }}
     j = i;
  */

  /* Basic Blocks :
     B1 = [int i; .. int i' = 0]
     B2 = [i' = i' + 1, if (i' < 20) goto {{ B2 }}]
     B3 = [j = i]
  */

  // Create Start Block
  E(env.StartBlock("entry"s));

  // Create mutable `i`
  E(env.CreateMutableVariable("i"s, FixpointType::GetIntType(), {}, format(env, "__value <= 2147483647"s )));

  // Create mutable variable `return`
  E(env.CreateMutableVariable("return"s, FixpointType::GetIntType(), {}, format(env, "__value == 5"s )));

  // Create mutable variable `i_one`
  E(env.CreateMutableVariable("i_one"s, FixpointType::GetIntType(), {}, format(env, "__value == 0"s )));

  // Jump to the next block
  E(env.AddJumpInformation("if.loop"s));
  
  // Create the "if.loop" block
  E(env.StartBlock("if.loop"s));

  // Assign to the mutable variable `temp` the current value of `i_one`
  E(env.CreatePhiNode("temp"s, FixpointType::GetIntType(), { "i_one"s, "i_one"s }, { "if.loop"s, "entry"s }))

  // Increment the value of `i_one`
  E(env.AssignMutableVariable("i_one"s, format(env, "__value == {{temp}} + 1"s)));

  // Check to see whether we should loop or break
  E(env.CreateMutableVariable("cmp"s, FixpointType::GetBoolType(), {}, format(env, "__value <=> {{i_one}} < 20"s)));

  // Add Branching information
  E(env.AddBranchInformation("cmp"s, true, "if.loop"s));
  E(env.AddBranchInformation("cmp"s, false, "if.end"s));

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
