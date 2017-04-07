#ifndef LLVM_TRANSFORMS_UTILS_METADATAPARSER_H
#define LLVM_TRANSFORMS_UTILS_METADATAPARSER_H

#include <string>
#include <vector>
#include "llvm/Pass.h"
#include "llvm/IR/Type.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata.h"

using namespace llvm;

namespace liquid {

	class RefinementMetadataParser {
	public:
		static ResultType ParseMetadata(RefinementMetadata& in, RefinementMetadata& out);
	};
}

#endif