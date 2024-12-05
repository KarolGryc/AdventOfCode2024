#include <iostream>
#include <utils.hpp>
#include "PageRules.hpp"
#include "Pages.hpp"

uint64_t sumCorrectMidPages(const std::vector<Pages>& pagesVec, const PageRules& rules);
uint64_t sumOfFixedMids(std::vector<Pages> pagesVec, const PageRules& rules);

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

		auto rules = PageRules::loadFromFile(rulesFileName);
		auto pagesVec = Pages::loadFromFile(pagesFileName);

		std::cout << "Sum of pages mids: " 
			<< sumCorrectMidPages(pagesVec, rules) << std::endl;

		std::cout << "Sum of fixed pages mids: " 
			<< sumOfFixedMids(pagesVec, rules) << std::endl;
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

uint64_t sumOfFixedMids(std::vector<Pages> pagesVec, const PageRules& rules)
{
	uint64_t sum{};
	for (auto& pages : pagesVec) {
		if (!pages.areValid(rules)) {
			pages.fix(rules);
			sum += pages.getMidPage();
		}
	}

	return sum;
}