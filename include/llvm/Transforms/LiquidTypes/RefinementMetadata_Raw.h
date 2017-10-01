#ifndef LLVM_TRANSFORMS_UTILS_METADATARAW_H
#define LLVM_TRANSFORMS_UTILS_METADATARAW_H

#include <string>
#include <vector>
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Transforms/LiquidFixpointBuilder/ResultType.h"

using namespace llvm;

namespace liquid {

	class RefinementMetadataForVariable_Raw
	{
	public:
		std::string OriginalName;
		std::string LLVMName;
		std::string OriginalType;
		llvm::Type* LLVMType;
		std::string Assume;
		std::string Verify;

		RefinementMetadataForVariable_Raw& operator=(const RefinementMetadataForVariable_Raw& other);
	};

	class RefinementMetadata_Raw {
	public:
		std::vector<std::string> Qualifiers;
		std::vector<RefinementMetadataForVariable_Raw> Parameters;
		RefinementMetadataForVariable_Raw Return;

		RefinementMetadata_Raw& operator=(const RefinementMetadata_Raw& other);

		static ResultType Extract(Function& F, RefinementMetadata_Raw& ret);
	private:
		static ResultType IncrementPointerAndGetString(const MDOperand* &operand, const MDOperand* operandEnd, std::string& str);
		static ResultType ParseRefinement(RefinementMetadata_Raw& refinementData, const MDOperand* &operand, const MDOperand* operandEnd);
	};
}

#endif
