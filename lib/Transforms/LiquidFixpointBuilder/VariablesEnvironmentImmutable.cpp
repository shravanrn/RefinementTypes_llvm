#include "llvm/Transforms/LiquidFixpointBuilder/VariablesEnvironmentImmutable.h"
#include "llvm/Transforms/LiquidFixpointBuilder/RefinementUtils.h"
#include <algorithm>
#include <cassert>

using namespace std::literals::string_literals;

namepsace liquid
{
  // Create an Immutable Variable
  ResultType VariablesEnvironment::CreateImmutableInputVariable
