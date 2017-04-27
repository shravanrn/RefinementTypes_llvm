#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONSIGNATUREINFO_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONSIGNATUREINFO_H

#include "llvm/Transforms/LiquidTypes/RefinementConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata_Raw.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"

namespace liquid {

	class RefinementFunctionSignatureInfo {
	public:
		RefinementMetadata_Raw FnRefinementMetadata_Raw;
		RefinementMetadata ParsedFnRefinementMetadata;

		RefinementFunctionSignatureInfo() = default;
	};

}
#endif