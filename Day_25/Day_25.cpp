#include <utils.hpp>

class Lock
{
public:
	Lock(size_t lockLength, int64_t lockHeight)
		: m_lockVals(lockLength, {}), m_lockHeight(lockHeight)
	{}

	void setLockVal(size_t pos, int64_t val)
	{
		m_lockVals[pos] = val;
	}

	size_t getLength() const
	{
		return m_lockVals.size();
	}

	size_t getHeight() const
	{
		return m_lockHeight;
	}

	int64_t getLockVal(size_t pos) const
	{
		return m_lockVals[pos];
	}

private:
	std::vector<int64_t> m_lockVals;
	int64_t m_lockHeight;
};

class Key
{
public:
	Key(size_t keyLength)
		: m_keyVals(keyLength)
	{}

	void setKeyVal(size_t pos, int64_t val)
	{
		m_keyVals[pos] = val;
	}

	int64_t getKeyVal(size_t pos) const
	{
		return m_keyVals[pos];
	}

	size_t getLength() const
	{
		return m_keyVals.size();
	}

	bool fits(const Lock& lock) const
	{
		if (lock.getLength() != this->getLength()) {
			return false;
		}

		auto n = lock.getLength();
		auto lockHeight = lock.getHeight();
		for (size_t i = 0; i < n; i++) {
			if (getKeyVal(i) >= lockHeight - lock.getLockVal(i)) {
				return false;
			}
		}

		return true;
	}

private:
	std::vector<int64_t> m_keyVals;
};

class KeyLockParser
{
public:
	std::vector<Key> parseKeys(const std::vector<std::string>& lines) const
	{
		std::vector<Key> result;
		for (auto begIt = lines.cbegin(); begIt != lines.cend(); begIt++) {
			if (begIt->empty()) {
				continue;
			}

			auto endIt = findElementEnd(begIt, lines.cend());
			if (isFullySolid(*begIt)) {
				Key parsed = parseKey(begIt, endIt);
				result.emplace_back(parsed);
			}
			begIt = endIt - 1;
		}

		return result;
	}

	std::vector<Lock> parseLocks(const std::vector<std::string>& lines) const
	{
		std::vector<Lock> result;
		for (auto begIt = lines.cbegin(); begIt != lines.cend(); begIt++) {
			if (begIt->empty()) {
				continue;
			}

			auto endIt = findElementEnd(begIt, lines.cend());
			if (isFullyEmpty(*begIt)) {
				Lock parsed = parseLock(begIt, endIt);
				result.emplace_back(parsed);
			}
			begIt = endIt - 1;
		}

		return result;
	}

	Key parseKey(
		std::vector<std::string>::const_iterator begin,
		const std::vector<std::string>::const_iterator& end) const
	{
		size_t keyLength = begin->size();
		Key parsedKey(keyLength);

		auto& curr = begin;
		while (curr != end) {
			if (containsIllegalChar(*curr)) {
				throw std::invalid_argument("Invalid key format");
			}

			if (curr->size() != keyLength) {
				throw std::runtime_error("Key input isn't rectangular");
			}

			for (size_t i = 0; i < keyLength; i++) {
				auto keyVal = parsedKey.getKeyVal(i);
				bool containsKey = isSolid(curr->at(i));

				if (containsKey) {
					parsedKey.setKeyVal(i, keyVal + 1);
				}
			}

			curr++;
		}

		return parsedKey;
	}

	Lock parseLock(
		std::vector<std::string>::const_iterator begin,
		const std::vector<std::string>::const_iterator& end) const
	{
		size_t lockLength = begin->size();
		int64_t lockHeight = std::distance(begin, end) + 1;
		Lock parsedLock(lockLength, lockHeight);

		auto& curr = begin;
		while (curr != end) {
			if (containsIllegalChar(*curr)) {
				throw std::invalid_argument("Invalid lock format");
			}

			if (curr->size() != lockLength) {
				throw std::runtime_error("Input lock isn't rectangular");
			}

			for (size_t i = 0; i < lockLength; i++) {
				auto lockVal = parsedLock.getLockVal(i);
				bool containsLock = isSolid(curr->at(i));

				if (containsLock) {
					parsedLock.setLockVal(i, lockVal + 1);
				}
			}

			curr++;
		}

		return parsedLock;
	}

private:
	auto findElementEnd(
		const std::vector<std::string>::const_iterator& begIt,
		const std::vector<std::string>::const_iterator& vecEnd) const
		-> std::vector<std::string>::const_iterator
	{
		std::vector<std::string>::const_iterator endIt = begIt;
		while (endIt != vecEnd && !endIt->empty()) {
			endIt++;
		}

		return endIt;
	}

	bool isFullySolid(const std::string& s) const
	{
		return s.find_first_not_of(solidSymbol) == std::string::npos;
	}

	bool isFullyEmpty(const std::string& s) const
	{
		return s.find_first_not_of(emptySymbol) == std::string::npos;
	}

	bool containsIllegalChar(const std::string& s) const
	{
		return s.find_first_not_of({emptySymbol, solidSymbol}) != std::string::npos;
	}

	bool isSolid(char c) const
	{
		return c == solidSymbol;
	}

	static constexpr char emptySymbol = '.';
	static constexpr char solidSymbol = '#';
};


class LocksSolution
{
public:
	uint64_t countFittingKeys(const std::vector<Lock>& locks, const std::vector<Key>& keys)
	{
		uint64_t cnt{};
		for (const auto& lock : locks) {
			for (const auto& key : keys) {
				if (key.fits(lock)) {
					cnt++;
				}
			}
		}

		return cnt;
	}

	uint64_t countFittingKeys(const Lock& lock, const std::vector<Key>& keys)
	{
		uint64_t cnt{};
		for (const auto& key : keys) {
			if (key.fits(lock)) {
				cnt++;
			}
		}

		return cnt;
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
	
	KeyLockParser parser;
	LocksSolution solution;
	for (const std::string& arg : runArgs) {
		try {
			static constexpr uint64_t numOfIterations = 2000;
			auto lines = aoc::loadFile(arg);
			auto keys = parser.parseKeys(lines);
			auto locks = parser.parseLocks(lines);
			std::cout << solution.countFittingKeys(locks, keys);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}