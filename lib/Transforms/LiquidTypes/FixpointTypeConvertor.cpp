#include "llvm/Transforms/LiquidTypes/FixpointTypeConvertor.h"

namespace liquid
{
	ResultType FixpointTypeConvertor::GetFixpointType(const llvm::Type& llvmType, FixpointBaseType& fixpointType)
	{
		if (llvmType.isIntegerTy())
		{
			fixpointType = (llvmType.getIntegerBitWidth() == 1) ? FixpointBaseType::BOOL : FixpointBaseType::INT;
			return ResultType::Success();
		}

		return ResultType::Error("Refinement Types: cannot convert type - " + llvmType.getStructName().str() + " to fixpoint type");
	}
}
