#include <utils.hpp>
#include <sstream>
#include <queue>

class FallingBytesParser
{
public:
	std::unordered_set<aoc::Position> parseFallingBytes(const std::vector<std::string>& lines, uint64_t parseFirst = UINT64_MAX) const
	{
		std::unordered_set<aoc::Position> bytePoisitions;
		uint64_t cnt = 0;
		for (auto& line : lines) {
			if (cnt >= parseFirst) {
				break;
			}
			aoc::Position bytePos = parseByte(line);
			bytePoisitions.insert(bytePos);
			++cnt;
		}

		return bytePoisitions;
	}

private:
	aoc::Position parseByte(const std::string& line) const
	{
		std::stringstream ss(line);
		int64_t x;
		int64_t y;
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
	uint64_t shortestExitPathLength(
		int64_t memSizeX,
		int64_t memSizeY,
		const std::unordered_set<aoc::Position>& corruptedPositions) const
	{
		printBoard(memSizeX, memSizeY, corruptedPositions);

		std::unordered_set<aoc::Position> visited;
		std::queue<std::pair<aoc::Position, uint64_t>> positionsToVisit;
		aoc::Position goal = { memSizeX - 1, memSizeY - 1 };
		positionsToVisit.push({ {0,0}, 0 });

		while (!positionsToVisit.empty()) {
			const aoc::Position currPos = positionsToVisit.front().first;
			const uint64_t cost = positionsToVisit.front().second;
			positionsToVisit.pop();
			if (currPos == goal) {
				return cost;
			}

			visited.insert(currPos);
			aoc::Vec2D offsets[] = { {1, 0}, { 0, 1 }, { -1, 0 }, { 0, -1 } };
			for (aoc::Vec2D uVec : offsets) {
				const aoc::Position newPos = currPos + uVec;
				if (memContains(newPos, memSizeX, memSizeY) && !visited.contains(newPos)) {
					if (!corruptedPositions.contains(newPos)) {
						positionsToVisit.push({newPos, cost + 1});
						std::cout << "Contains " << visited.contains(newPos) << " ";
						std::cout << "Pushing " << newPos.x << " " << newPos.y << std::endl;
					}
				}
			}
		}

		return UINT64_MAX;
	}

private:
	bool memContains(const aoc::Position& p, int64_t sizeX, int64_t sizeY) const 
	{
		return p.y >= 0 && p.y < sizeY && p.x >= 0 && p.x < sizeX;
	}

	void printBoard(
		int64_t memSizeX,
		int64_t memSizeY,
		const std::unordered_set<aoc::Position>& corruptedPositions) const
	{
		for (int64_t i = 0; i < memSizeY; i++) {
			for (int64_t j = 0; j < memSizeX; j++) {
				if (corruptedPositions.contains({ i,j })) {
					std::cout << "# ";
				}
				else {
					std::cout << ". ";
				}
			}
			std::cout << std::endl;
		}
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
			constexpr int64_t memSizeX = 71;
			constexpr int64_t memSizeY = 71;
			constexpr uint64_t parsedBytesNum = 1024;
			auto lines = aoc::loadFile(arg);
			auto bytes = parser.parseFallingBytes(lines, parsedBytesNum);
			std::cout 
				<< solution.shortestExitPathLength(memSizeX, memSizeY, bytes)
				<< std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}