#ifndef LLVM_TRANSFORMS_UTILS_METADATA_H
#define LLVM_TRANSFORMS_UTILS_METADATA_H

#include <string>
#include <vector>
#include "llvm/IR/Type.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Function.h"
#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/RefinementMetadata_Raw.h"

using namespace llvm;

namespace liquid {

	class RefinementMetadataForVariable
	{
	public:
		std::string OriginalName;
		std::string LLVMName;
		std::string OriginalType;
		llvm::Type* LLVMType;
		std::string Assume;
		std::string Verify;

		RefinementMetadataForVariable& operator=(const RefinementMetadataForVariable& other);
	};

	class RefinementMetadata {
	public:
		std::vector<std::string> Qualifiers;
		std::vector<RefinementMetadataForVariable> Parameters;
		RefinementMetadataForVariable Return;

		RefinementMetadata& operator=(const RefinementMetadata& other);

		static ResultType ParseMetadata(RefinementMetadata_Raw& in, RefinementMetadata& out);
	};
}

#endif