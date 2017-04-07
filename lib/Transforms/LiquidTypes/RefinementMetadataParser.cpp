#include "llvm/Transforms/LiquidTypes/RefinementMetadataParser.h"


using namespace std::string_literals;
using namespace liquid;
using namespace llvm;

ResultType RefinementMetadataParser::ParseMetadata(RefinementMetadata& in, RefinementMetadata& out)
{
	out = in;
	return ResultType::Success();
}
