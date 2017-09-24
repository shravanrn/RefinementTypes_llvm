#include <regex>
#include <iostream>

#include "llvm/Transforms/LiquidTypes/VariablesEnvironment.h"
using namespace liquid;

std::string format(VariablesEnvironment& env, std::string expression)
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

int ifExample();
int ifThenElseExample();
int forLoopExample();

int main()
{
	int a; 
	std::cout << "Enter a number" << std::endl;
	std::cout << "1 for example with if" << std::endl;
	std::cout << "2 for example with if else" << std::endl;
	std::cout << "3 for example with for loop" << std::endl;
	std::cout << "Choice: ";
	std::cin >> a;

	if(a == 1)
	{
		return ifExample();
	}
        else if(a == 2)
	{
		return ifTheElseExample();
	}
	else if(a == 3)
	{
		return forLoopExample();
	}
	else
	{
		std::cout << "Example not available" << std::endl;
		return -1;
	}
}
