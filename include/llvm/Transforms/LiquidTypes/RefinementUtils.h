#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSUTILS_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSUTILS_H

#include <string>
#include <vector>

namespace liquid {

	/// A pass which verifies refinements 
	class RefinementUtils {
	public:

		template <typename T>
		static std::string stringJoin(std::string separator, std::vector<T> strings)
		{
			std::stringstream outputBuff;
			bool first = true;
			for (auto& s : strings)
			{
				if (first)
				{
					first = false;
				}
				else
				{
					outputBuff << separator;
				}

				outputBuff << s;
			}

			return outputBuff.str();
		}

		template <typename T>
		static std::vector<T> vectorAppend(std::vector<T>& vectorA, std::vector<T>& vectorB)
		{
			std::vector<T> result;
			result.reserve(vectorA.size() + vectorB.size());
			result.insert(result.end(), vectorA.begin(), vectorA.end());
			result.insert(result.end(), vectorB.begin(), vectorB.end());
			return result;
		}
	};
}

#endif