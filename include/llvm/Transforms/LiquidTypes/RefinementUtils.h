#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSUTILS_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSUTILS_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace liquid {

	/// A pass which verifies refinements 
	class RefinementUtils {
	public:

		template <typename T>
		static std::string stringJoin(const std::string& separator, const std::vector<T>& strings)
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
		static std::vector<T> vectorAppend(const std::vector<T>& vectorA, const std::vector<T>& vectorB)
		{
			std::vector<T> result;
			result.reserve(vectorA.size() + vectorB.size());
			result.insert(result.end(), vectorA.begin(), vectorA.end());
			result.insert(result.end(), vectorB.begin(), vectorB.end());
			return result;
		}

		template <typename T1, typename T2>
		static bool containsKey(const std::map<T1, T2>& m, const T1& key)
		{
			auto it = m.find(key);
			bool found = (it != m.end());
			return found;
		}

		template<typename T1, typename T2>
		static auto selectString(const std::vector<T1>& source, T2 projectFunc)
		{
			std::vector<std::string> ret;
			for (auto& el : source)
			{
				ret.push_back(projectFunc(el));
			}

			return ret;
		}
	};
}

#endif