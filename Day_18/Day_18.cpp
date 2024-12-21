#include <utils.hpp>
#include <sstream>
#include <queue>

class FallingBytesParser
{
public:
	std::vector<aoc::Position> parseFallingBytes(const std::vector<std::string>& lines) const
	{
		std::vector<aoc::Position> bytePoisitions;
		for (auto& line : lines) {
			aoc::Position bytePos = parseByte(line);
			bytePoisitions.push_back(bytePos);
		}

		return bytePoisitions;
	}

private:
	aoc::Position parseByte(const std::string& line) const
	{
		std::stringstream ss(line);
		int64_t x,y;
		char separator;
		ss >> x >> separator >> y;

		if (ss.fail()) {
			throw std::invalid_argument("Ill formated byte.");
		}

		return { x,y };
	}
};

class MemoryPathFinder
{
public:
	uint64_t exitPathLength(
		int64_t memSizeX,
		int64_t memSizeY,
		const std::unordered_set<aoc::Position>& corruptedPositions) const
	{
		static constexpr aoc::Position startPos{ 0,0 };
		static constexpr uint64_t startCost{};
		std::unordered_set<aoc::Position> included{startPos};
		std::queue<std::pair<aoc::Position, uint64_t>> positionsToVisit{ {{startPos, startCost}} };
		aoc::Position goal = { memSizeX - 1, memSizeY - 1 };

		while (!positionsToVisit.empty()) {
			const auto& [currPos, cost] = positionsToVisit.front();
			if (currPos == goal) {
				return cost;
			}

			for (auto& uVec : aoc::Vec2D::getUnitVectors()) {
				const auto newPos = currPos + uVec;
				if (memContains(newPos, memSizeX, memSizeY) && 
					!included.contains(newPos) &&
					!corruptedPositions.contains(newPos))
				{
					positionsToVisit.push({newPos, cost + 1});
					included.insert(newPos);
				}
			}
			positionsToVisit.pop();
		}

		return UINT64_MAX;
	}

	aoc::Position firstBlockingByte(
		int64_t memSizeX,
		int64_t memSizeY,
		const std::vector<aoc::Position>& corruptedPositions) const
	{
		std::unordered_set<aoc::Position> fallenBytes{};
		for (auto& bytePos : corruptedPositions) {
			fallenBytes.insert(bytePos);
			uint64_t exitPathLen = exitPathLength(memSizeX, memSizeY, fallenBytes);
			if (exitPathLen == UINT64_MAX) {
				return bytePos;
			}
		}

		throw std::exception("No blocking byte found");
	}

private:
	bool memContains(const aoc::Position& p, int64_t sizeX, int64_t sizeY) const 
	{
		return p.y >= 0 && p.y < sizeY && p.x >= 0 && p.x < sizeX;
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

	FallingBytesParser parser;
	MemoryPathFinder solution;
	for (const std::string& arg : runArgs) {
		try {
			constexpr int64_t mSizeX = 71;
			constexpr int64_t mSizeY = 71;
			constexpr uint64_t fallenBytesNum = 1024;
			auto lines = aoc::loadFile(arg);
			auto bytes = parser.parseFallingBytes(lines);
			std::unordered_set<aoc::Position> p1BytesSet =
				{ bytes.begin(), bytes.begin() + fallenBytesNum };

			std::cout << "Shortest exit path length for part 1: "
				<< solution.exitPathLength(mSizeX, mSizeY, p1BytesSet)
				<< std::endl;

			auto blockingByte = solution.firstBlockingByte(mSizeX, mSizeY, bytes);
			std::cout << "First blocking byte: "
				<< blockingByte.x << "," << blockingByte.y
				<< std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}