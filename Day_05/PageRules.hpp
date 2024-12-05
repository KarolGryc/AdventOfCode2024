#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>

class PageRules
{
public:
	static PageRules loadFromFile(const std::string& fileName);

	void addRule(int ruleFor, int mustBeBefore);
	const std::unordered_set<int>& getConstraintsFor(int pageNum) const;

private:
	std::unordered_map<int, std::unordered_set<int>> m_rules;
	std::unordered_set<int> m_emptySet;
};

