#include "Pages.hpp"
#include "utils.hpp"
#include <sstream>

std::vector<Pages> Pages::loadFromFile(const std::string& fileName)
{
	std::vector<std::string> lines{ aoc::loadFile(fileName) };
	std::vector<Pages> pagesVec;

	for (std::string& line : lines) {
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

bool Pages::areValid(const PageRules& rules) const
{
	return firstBadElementIdx(rules) == -1;
}

void Pages::fix(const PageRules& rules)
{
	// Could be much more optimised, but we have to save Christmas right now!
	int badIdx;
	while ((badIdx = firstBadElementIdx(rules)) != -1) {
		int newIdx = firstReplacementIdx(badIdx, rules);
		swapPages(badIdx, newIdx);
	}
}

int Pages::getMidPage() const
{
	return (*this)[this->size() / 2];
}

int Pages::firstBadElementIdx(const PageRules& rules) const
{
	std::unordered_set<int> numsAfterCurr{ begin() + 1, end() };
	for (int i = 0; i < size() - 1; ++i) {
		int pageNum = at(i);
		const auto& requiredBefore = rules.getConstraintsFor(pageNum);
		if (aoc::setsIntersect(requiredBefore, numsAfterCurr)) {
			return i;
		}

		numsAfterCurr.erase(pageNum);
	}
	return -1;
}

int Pages::firstReplacementIdx(int numIdx, const PageRules& rules) const
{
	int num = at(numIdx);
	std::unordered_set<int> numsAfter{ begin() + numIdx, end() };
	const auto& requiredBefore = rules.getConstraintsFor(num);

	int newIdx = numIdx;
	while (aoc::setsIntersect(numsAfter, requiredBefore)) {
		numsAfter.erase(at(newIdx++));
	}
	return std::min(newIdx - 1, (int)size());
}

void Pages::swapPages(int firstIdx, int secondIdx)
{
	std::swap(at(firstIdx), at(secondIdx));
}