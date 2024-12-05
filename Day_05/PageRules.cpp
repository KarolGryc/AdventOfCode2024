#include "PageRules.hpp"
#include <sstream>
#include "utils.hpp"

PageRules PageRules::loadFromFile(const std::string& fileName)
{
	std::vector<std::string> lines{ aoc::loadFile(fileName) };

	PageRules rules;
	for (const std::string& line : lines) {
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

const std::unordered_set<int>& PageRules::getConstraintsFor(int pageNum) const
{
	if (m_rules.contains(pageNum)) {
		return m_rules.at(pageNum);
	}
	else {
		return m_emptySet;
	}
}


void PageRules::addRule(int ruleFor, int mustBeBefore)
{
	m_rules[ruleFor].insert(mustBeBefore);
}