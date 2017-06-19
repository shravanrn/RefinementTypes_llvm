#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTTYPE_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTTYPE_H

#include <string>

namespace liquid
{
	class FixpointType
	{
		std::string FixpointTypeString;

	public:

		FixpointType() {}
		FixpointType(const std::string& fixpointTypeString) : FixpointTypeString(fixpointTypeString) {}
		FixpointType(const FixpointType& other) :FixpointTypeString(other.FixpointTypeString) {}

		std::string ToString() const
		{
			return FixpointTypeString;
		}

		static FixpointType GetIntType()
		{
			return FixpointType("int");
		}

		static FixpointType GetBoolType()
		{
			return FixpointType("bool");
		}

		static FixpointType GetPointerType()
		{
			return FixpointType("int");
		}

		bool operator==(const FixpointType& rhs) const
		{
			return (FixpointTypeString == rhs.FixpointTypeString);
		}

		bool operator!=(const FixpointType& rhs) const
		{
			return !operator==(rhs);
		}
	};
}

#endif