#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
#include "llvm/Transforms/LiquidTypes/FunctionBlockgraph.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include <regex>
#include <iostream>

using namespace liquid;
using namespace std::literals::string_literals;

class LLVMFunctionBlockGraph_IfTwo: public FunctionBlockGraph
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
  if (!a.Succeeded) { str = a.ErrorMsg goto errh;}\
}

int ifThenElseExample()
{
  /* Code to Verify :
     int a = 4;
     int b;
     if (a == 4) { b = 5; } else { b = 6; }
     int b {__value == 5}
  */
  LLVMFunctionBlockGraph_If llvmFunctionBlockGraph;
  Variablesenvironment env(llvmFunctionBlockGraph);

  std::string str;

  /* Corresponding 3-address code :
     int a = 4;
     int b;
     if (a == 4) goto {{ return 5; }}
     return 6;
     goto {{ terminal }}
     return 5;
     terminal;
  */

  /* Basic Blocks : 
     B1 = [int a =4; .. return 6;]
     B2 = [ return 5; ]
     B3 = [ goto {{ terminal }} ]
  */

  // Create Start Block 
  E(env.StartBlock("entry"s));

  // Create immutable `a`, such that, `a` = 4
  E(env.CreateImmutableVariable("a"s, FixpointType::GetIntType(), { "__value == 4"}));

  // Create mutable variable `return`
  E(env.CreateMutableOutputVariable("return"s, FixpointType::GetIntType(), { "__value == 6"}));

  // Create mutable variable `b`
  E(env.CrateMutableVariable("b"s, FixpointType::GetIntType(), { "__value < 7" }));
    
  // Create mutable variable `cmp` for branching information
  E(env.CreateMutableVariable("cmp"s, FixpointType::GetBoolType(), {}, format(env, "__value <=> {{a}} == 4"s)));

  // Add Branching information
  E(env.AddBranchInformation("cmp"s, true, "if.then"s));
  E(env.AddBranchInformation("cmp"s, false, "if.end"s));

  // Create the "if.then" block
  E(env.StartBlock("if.then"s));
  E(env.AssignMutableVariable("b"s, format(env, "__value == 5"s)));
  E(env.addJumpInformation("if.end"s));

  // Create the "if.end" block 
  E(env.StartBlock("if.end"s));
  // Create the Phi-Node
  E(env.CreatePhiNode("d"s, FixpointType::GetIntType(), { "return"s, "b"s }, { "entry"s, "if.then"s }));
  E(env.AssignMutableVariable("return", format(env, "__value = {{d}}"s)));

  // Check for failure, and verify this works; or fail and be loud
  E(env.ToStringOrFailure(str));

  std::cout << "Succeeded:" << std::endl << str;
  return 0;

errh:
  std::cout << "Failed:" << std::endl << str;
  return -1;
}
  
  
  
  
