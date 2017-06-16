#ifndef LLVM_TRANSFORMS_UTILS_REFINEMENTSUTILS_H
#define LLVM_TRANSFORMS_UTILS_REFINEMENTSUTILS_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <set>
#include <iterator>

namespace liquid {

	class RefinementUtils {
	public:

		template <typename T>
		static std::string StringJoin(const std::string& separator, const std::vector<T>& strings)
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
		static inline std::vector<T> VectorAppend(const std::vector<T>& vectorA, const std::vector<T>& vectorB)
		{
			std::vector<T> result;
			result.reserve(vectorA.size() + vectorB.size());
			result.insert(result.end(), vectorA.begin(), vectorA.end());
			result.insert(result.end(), vectorB.begin(), vectorB.end());
			return result;
		}

		template <typename T>
		static inline std::set<T> SetAppend(const std::set<T>& setA, const std::set<T>& setB)
		{
			std::set<T> result;
			result.insert(setA.begin(), setA.end());
			result.insert(setB.begin(), setB.end());
			return result;
		}

		template <typename T>
		static inline std::set<T> SetAppend(const std::vector<T>& vectorA, const std::vector<T>& vectorB)
		{
			std::set<T> result;
			result.insert(vectorA.begin(), vectorA.end());
			result.insert(vectorB.begin(), vectorB.end());
			return result;
		}

		template <typename T>
		static inline std::set<T> SetIntersection(const std::set<T>& setA, const std::set<T>& setB)
		{
			std::set<T> result;
			std::set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(), std::inserter(result, result.begin()));
			return result;
		}

		template <typename T>
		static inline std::set<T> SetDifference(const std::set<T>& setA, const std::set<T>& setB)
		{
			std::set<T> result;
			std::set_difference(setA.begin(), setA.end(), setB.begin(), setB.end(), std::inserter(result, result.begin()));
			return result;
		}

		template <typename T1, typename T2>
		static inline bool ContainsKey(const std::map<T1, T2>& m, const T1& key)
		{
			auto it = m.find(key);
			bool found = (it != m.end());
			return found;
		}

		template <typename Container, typename ElementType>
		static inline bool Contains(Container sourceContainer, ElementType element)
		{
			return std::find(sourceContainer.begin(), sourceContainer.end(), element) != sourceContainer.end();
		}

		template<typename T1, typename T2>
		static auto SelectString(const std::vector<T1>& source, T2 projectFunc)
		{
			std::vector<std::string> ret;
			for (auto& el : source)
			{
				ret.push_back(projectFunc(el));
			}

			return ret;
		}

		template <typename T1, typename T2>
		static inline std::vector<T1> GetKeys(const std::map<T1, T2>& m)
		{
			std::vector<T1> ret;
			std::transform(m.begin(), m.end(), std::back_inserter(ret), [](const std::map<T1, T2>::value_type& val) { return val.first; });
			return ret;
		}

		template <typename T1, typename T2>
		static inline std::set<T1> GetKeysSet(const std::map<T1, T2>& m)
		{
			std::set<T1> ret;
			std::transform(m.begin(), m.end(), std::inserter(ret, ret.begin()), [](const std::map<T1, T2>::value_type& val) { return val.first; });
			return ret;
		}

		template <typename T1, typename T2>
		static inline std::vector<T2> GetValues(const std::map<T1, T2>& m)
		{
			std::vector<T2> ret;
			std::transform(m.begin(), m.end(), std::back_inserter(ret), [](const std::map<T1, T2>::value_type& val) { return val.second; });
			return ret;
		}
	};
}

#endif