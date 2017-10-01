#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTTYPECONVERTOR_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTTYPECONVERTOR_H

#include "llvm/IR/Type.h"
#include "llvm/Transforms/LiquidFixpointBuilder/ResultType.h"
#include "llvm/Transforms/LiquidFixpointBuilder/FixpointConstraintBuilder.h"

namespace liquid
{
	class FixpointTypeConvertor
	{
	public:
		ResultType GetFixpointType(const llvm::Type& llvmType, FixpointType& fixpointType);
	};
}

#endif
