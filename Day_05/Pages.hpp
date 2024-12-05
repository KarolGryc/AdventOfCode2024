#pragma once
#include <vector>
#include "PageRules.hpp"

class Pages : public std::vector<int>
{
public:
	// would be better with separate class FileLoader or something like that
	static std::vector<Pages> loadFromFile(const std::string& fileName);

	bool areValid(const PageRules& rules) const;
	void fix(const PageRules& rules);
	int getMidPage() const;

private:
	int firstBadElementIdx(const PageRules& rules) const;
	int firstReplacementIdx(int numIdx, const PageRules& rules) const;
	void swapPages(int firstIdx, int secondIdx);
};