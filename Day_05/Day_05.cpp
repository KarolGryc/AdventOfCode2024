#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <utils.hpp>

class PageRules 
{
public:
	void displayRules() {
		for (auto& [key, val] : m_rules) {
			std::cout << key << ":\n";
			for (auto el : val) {
				std::cout << "\t" << el << " ";
			}
			std::cout << std::endl;
		}
	}

	void addRule(int destinationPageNum, int requiredBeforePageNum)
	{
		m_rules[destinationPageNum].insert(requiredBeforePageNum);
	}

	const std::unordered_set<int>& getConstraintsFor(int pageNum) const
	{
		if (m_rules.contains(pageNum)) {
			return m_rules.at(pageNum);
		}
		else {
			return m_emptySet;
		}
	}

private:
	std::unordered_map<int, std::unordered_set<int>> m_rules;
	std::unordered_set<int> m_emptySet;
};

class Pages : public std::vector<int>
{
public:
	bool areValid(const PageRules& rules) const
	{
		std::unordered_set<int> numsAfterCurr{ this->begin(), this->end()};
		for(auto pageNum : *this) {
			const auto& requiredBefore = rules.getConstraintsFor(pageNum);
			for (int num : numsAfterCurr)
			{
				if (requiredBefore.contains(num))
					return false;
			}
			numsAfterCurr.erase(pageNum);
		}
		return true;
	}

	int getMidPage() const
	{
		return (*this)[this->size() / 2];
	}
};

PageRules loadRulesFromFile(const std::string& fileName);
std::vector<Pages> loadPagesFromFile(const std::string& fileName);
uint64_t sumCorrectMidPages(const std::vector<Pages>& pagesVec, const PageRules& rules);


int main(int argc, char* args[])
{
	std::vector<std::string> runArgs = aoc::argsToString(argc - 1, args + 1);

	if (runArgs.size() != 2) {
		std::cerr << "Number of given files isn't equal to two" << std::endl;
		return 1;
	}

	try {
		const std::string& rulesFileName = runArgs[0];
		const std::string& pagesFileName = runArgs[1];

		PageRules rules{ loadRulesFromFile(rulesFileName) };
		std::vector<Pages> pagesVec{ loadPagesFromFile(pagesFileName) };

		rules.displayRules();

		uint64_t sum = sumCorrectMidPages(pagesVec, rules);
		std::cout << "Sum of mid pages: " << sum;

		sum = sumCorrectMidPages(pagesVec, rules);
		std::cout << "Sum of mid pages: " << sum;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}


uint64_t sumCorrectMidPages(const std::vector<Pages>& pagesVec, const PageRules& rules)
{
	uint64_t sum{};
	for (auto& pages : pagesVec) {
		sum += pages.areValid(rules) ? pages.getMidPage() : 0;
	}

	return sum;
}


PageRules loadRulesFromFile(const std::string& fileName)
{
	std::vector<std::string> lines{ aoc::loadFile(fileName) };

	PageRules rules;
	for (const std::string& line : lines) {
		if (line.empty()) {
			continue;
		}

		std::stringstream ss(line);
		char separator;
		int mustBePrintedBefore, mustBePrintedAfter;
		if (ss >> mustBePrintedBefore >> separator >> mustBePrintedAfter) {
			rules.addRule(mustBePrintedAfter, mustBePrintedBefore);
		}
		else {
			throw std::runtime_error("Rules file " + fileName + " is damaged");
		}
	}

	return rules;
}

std::vector<Pages> loadPagesFromFile(const std::string& fileName)
{
	std::vector<std::string> lines{ aoc::loadFile(fileName) };
	
	std::vector<Pages> pagesVec;
	for (std::string& line : lines) {
		if (line.empty()) {
			continue;
		}

		if (line.back() != ',') {
			line.push_back(',');
		}

		std::stringstream ss(line);
		Pages pages;
		int pageNum;
		char separator;
		while (ss >> pageNum >> separator) {
			pages.push_back(pageNum);
		}
		if (ss.bad()) {
			throw std::runtime_error("Pages file " + fileName + " is damaged");
		}

		pagesVec.emplace_back(std::move(pages));
	}

	return pagesVec;
}
