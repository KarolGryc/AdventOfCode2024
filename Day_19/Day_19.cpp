#include <utils.hpp>
#include <regex>
#include <stack>
#include <unordered_map>

class TowelParser
{
public:
	std::vector<std::string> parseAvailableTowels(const std::vector<std::string>& lines) const
	{
		if (lines.size() == 0) {
			throw std::invalid_argument("No lines given to parse.");
		}

		const std::string& line = lines.front();
		std::regex towelRegex(R"([uwbrg]+)");

		std::vector<std::string> towels;
		auto begin = std::sregex_iterator(line.begin(), line.end(), towelRegex);
		auto end = std::sregex_iterator();

		while (begin != end) {
			towels.push_back((*begin++).str());
		}

		return towels;
	}

	std::vector<std::string> parsePatterns(const std::vector<std::string>& lines) const
	{
		std::regex patternRegex(R"([uwbrg]+)");
		std::vector<std::string> patterns;
		for (auto it = lines.begin() + 2; it != lines.end(); ++it) {
			if (!std::regex_match(*it, patternRegex)) {
				throw std::invalid_argument("Ill formated towel pattern.");
			}

			patterns.push_back(*it);
		}
		return patterns;
	}
};



class TowelsSolution
{
public:
	uint64_t countPossible(
		const std::vector<std::string>& availableTowels,
		const std::vector<std::string>& patterns) const
	{
		uint64_t count{};
		for (const auto& pattern : patterns) {
			count += isPatternPossible(availableTowels, pattern);
		}

		return count;
	}


	uint64_t countWaysToAchieve(
		const std::vector<std::string>& availableTowels,
		const std::vector<std::string>& patterns) const
	{
		uint64_t count{};
		std::unordered_map<std::string, uint64_t> cache;
		for (const auto& pattern : patterns) {
			count += countWaysToAchieve(availableTowels, pattern.cbegin(), pattern.cend(), cache);
		}

		return count;
	}


	bool isPatternPossible(
		const std::vector<std::string>& availableTowels,
		const std::string& pattern) const
	{
		std::stack<std::string::const_iterator> stack({ pattern.cbegin() });
		const std::string::const_iterator end = pattern.cend();

		while (!stack.empty()) {
			const std::string::const_iterator begin = stack.top();
			stack.pop();
			for (const auto& towel : availableTowels) {
				if (patternBeginsWith(begin, end, towel)) {
					auto newBegin = begin + towel.size();
					if (newBegin == end) {
						return true;
					}
					else {
						stack.push(newBegin);
					}
				}
			}
		}

		return false;
	}


	[[deprecated]] // slow, memory efficient
	uint64_t __slow__countWaysToAchieve(
		const std::vector<std::string>& availableTowels,
		const std::string& pattern) const
	{
		uint64_t count{};
		std::stack<std::string::const_iterator> stack({ pattern.cbegin() });
		const std::string::const_iterator end = pattern.cend();

		while (!stack.empty()) {
			const std::string::const_iterator begin = stack.top();
			stack.pop();
			for (const auto& towel : availableTowels) {
				if (patternBeginsWith(begin, end, towel)) {
					auto newBegin = begin + towel.size();
					if (newBegin == end) {
						++count;
					}
					else {
						stack.push(newBegin);
					}
				}
			}
		}

		return count;
	}


private:
	uint64_t countWaysToAchieve(
		const std::vector<std::string>& towels,
		const std::string::const_iterator& begin,
		const std::string::const_iterator& end,
		std::unordered_map<std::string, uint64_t>& dp) const
	{
		uint64_t count{};
		for (const auto& towel : towels) {
			if (patternBeginsWith(begin, end, towel)) {
				auto newBegin = begin + towel.size();
				std::string subPattern{ newBegin, end };
				if(newBegin == end){
					dp[subPattern] = 1;
				}
				else if(!dp.contains(subPattern)){
					dp[subPattern] = countWaysToAchieve(towels, newBegin, end, dp);
				}

				count += dp[subPattern];
			}
		}
		return count;
	}


	bool patternBeginsWith(
		std::string::const_iterator patternBegin,
		const std::string::const_iterator& patternEnd,
		const std::string& towel) const
	{
		int64_t patternSize = std::distance(patternBegin, patternEnd);
		if (patternSize < towel.size()) {
			return false;
		}

		for (auto c : towel) {
			if (*(patternBegin++) != c) {
				return false;
			}
		}

		return true;
	}
};



int main(int argc, char* args[])
{
	auto runArgs = aoc::argsToString(argc - 1, args + 1);

	if (runArgs.empty()) {
		const std::string defaultFile = "input.txt";
		std::cerr << "No args given, running default file: " 
			<< defaultFile 
			<< std::endl;
		runArgs.push_back(defaultFile);
	}
	
	TowelParser parser;
	TowelsSolution solution;
	for (const std::string& arg : runArgs) {
		try {
			auto lines = aoc::loadFile(arg);
			auto towels = parser.parseAvailableTowels(lines);
			auto patterns = parser.parsePatterns(lines);

			std::cout << "Possible patterns num: "
				<< solution.countPossible(towels, patterns)
				<< std::endl
				<< "Num of ways to achieve all patterns: "
				<< solution.countWaysToAchieve(towels, patterns);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}