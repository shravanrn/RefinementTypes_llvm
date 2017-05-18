#include "llvm/Transforms/LiquidTypes/FixpointTypeConvertor.h"

namespace liquid
{
	ResultType FixpointTypeConvertor::GetFixpointType(const llvm::Type& llvmType, FixpointType& fixpointType)
	{
		if (llvmType.isIntegerTy())
		{
			fixpointType = (llvmType.getIntegerBitWidth() == 1) ? FixpointType::GetBoolType() : FixpointType::GetIntType();
			return ResultType::Success();
		}

		return ResultType::Error("Refinement Types: cannot convert type - to fixpoint type");
	}
}
