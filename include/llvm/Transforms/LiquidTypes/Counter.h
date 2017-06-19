#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_COUNTER_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_COUNTER_H

#include <string>

namespace liquid
{
	class Counter
	{
		unsigned int count = 1;
	public:
		inline unsigned int GetNextId() { unsigned int ret = count; count++; return ret; }
		inline unsigned int PeekNextId() const { return count; }
		inline std::string GetNextIdString() { unsigned int ret = GetNextId(); return std::to_string(ret); }
	};
}

#endif