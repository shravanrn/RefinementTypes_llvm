#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONINFO_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONINFO_H

#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"

namespace liquid {

	class RefinementFunctionInfo {
	public:
		RefinementMetadata FnRefinementMetadata;
		RefinementMetadata ParsedFnRefinementMetadata;
	};

}
#endif