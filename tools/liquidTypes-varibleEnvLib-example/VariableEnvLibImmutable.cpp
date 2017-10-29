#include <regex>
#include <iostream>

#include "llvm/Transforms/LiquidFixpointBuilder/VariablesEnvironmentImmutable.h"
using namespace liquid;

std::string format(VariablesEnvironmentImmutable& env, std::string expression)
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

// int ifImmutableExample();
// int ifThenElseExample();
// int forLoopExample();

int main()
{
  int a; 
  std::cout << "Enter a number" << std::endl;
  std::cout << "1 for (immutable) example with if" << std::endl;
  std::cout << "Choice: ";
  std::cin >> a;
  
  if(a == 1)
  {
    // return ifImmutableExample();
    return 0;
  }
  else
  {
      std::cout << "Example not available" << std::endl;
      return -1;
  }
}
