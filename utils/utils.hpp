#include <vector>
#include <string>
#include <unordered_set>
namespace aoc{
	std::vector<std::string> argsToString(int argc, char* args[]);
	std::vector<std::string> loadFile(const std::string& fileName);

	template<typename T>
	bool setsIntersect(const std::unordered_set<T>& s1, const std::unordered_set<T>& s2)
	{
		for (auto& el : s1) {
			if (s2.contains(el)) {
				return true;
			}
		}
		return false;
	}
}