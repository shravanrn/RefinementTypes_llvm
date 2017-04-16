#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONINFO_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSFUNCTIONINFO_H

#include "llvm/Transforms/LiquidTypes/RefinementConstraintGenerator.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata_Raw.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"

#include <memory>

namespace liquid {

	class RefinementFunctionInfo {
	public:
		bool RefinementDataFound = false;
		RefinementMetadata_Raw FnRefinementMetadata_Raw;
		RefinementMetadata ParsedFnRefinementMetadata;
		std::unique_ptr<RefinementConstraintGenerator> ConstraintGenerator;

		RefinementFunctionInfo() = default;
	};

}
#endif