#include <utils.hpp>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <numeric>
#include <execution>

using namespace aoc;

class HeightMap
{
public:
	using Height = int32_t;
	static constexpr Height HEIGHT_MAX = INT32_MAX;
	static constexpr Height trialhead = 0;
	static constexpr Height finishHeihgt = 9;

	HeightMap() = default;

	HeightMap(const std::vector<std::vector<Height>>& heights)
		: m_heights{heights}
	{}

	HeightMap(std::vector<std::vector<Height>>&& heights)
		: m_heights{std::move(heights)}
	{}

	std::vector<Position> getTrialheads() const {
		std::vector<Position> positions;
		for (int y = 0; y < sizeY(); ++y) {
			for (int x = 0; x < sizeX(y); ++x) {
				Position field{ x,y };
				Height fieldHeight = at(field);
				if (fieldHeight == trialhead) {
					positions.push_back({ x,y });
				}
			}
		}

		return positions;
	}

	Height at(Position p) const {
		return contains(p) ? m_heights[p.y][p.x] : HEIGHT_MAX;
	}

	bool isTrialHead(Position p) const {
		return at(p) == trialhead;
	}

	bool isFinishPoint(Position p) const {
		return at(p) == finishHeihgt;
	}

	bool contains(Position p) const {
		return p.y >= 0 && p.y < sizeY() && p.x >= 0 && p.x < sizeX(p.y);
	}

	size_t sizeX(int row = 0) const {
		return m_heights.at(0).size();
	}

	size_t sizeY() const {
		return m_heights.size();
	}

	void display() {
		for (auto r : m_heights) {
			for (auto e : r) {
				if (e < HEIGHT_MAX) {
					std::cout << e << " ";
				}
				else {
					std::cout << ". ";
				}
			}
			std::cout << std::endl;
		}
	}
private:
	std::vector<std::vector<Height>> m_heights;
};

class HeihgtMapParser 
{
public:
	HeightMap parseHeightMap(const std::vector<std::string>& lines)
	{
		if (lines.empty()) {
			return {};
		}

		std::vector<std::vector<HeightMap::Height>> heights;
		for (const std::string& line : lines) {
			std::vector<HeightMap::Height> row;
			for (char c : line) {
				if (isdigit(c))
					row.push_back(atoi(&c));
				else
					row.push_back(HeightMap::HEIGHT_MAX);
			}
			heights.push_back(std::move(row));
		}

		return HeightMap{ std::move(heights) };
	}
};


class HeihgtMapPathFinder
{
public:
	uint32_t trialheadsScoreSum(const HeightMap& map) const 
	{
		auto heads = map.getTrialheads();
		return std::transform_reduce(std::execution::par,
			heads.begin(), heads.end(), 0ull,
			std::plus<uint64_t>(),
			[this, &map](const Position& p) -> uint64_t {
				return trialheadScore(map, p);
		});
	}

	uint32_t trialheadsRatingsSum(const HeightMap& map) const {
		auto heads = map.getTrialheads();
		return std::transform_reduce(std::execution::par,
			heads.begin(), heads.end(), 0ull,
			std::plus<uint64_t>(),
			[this, &map](const Position& p) -> uint64_t {
				return trialheadRating(map, p);
			});
	}

private:
	uint32_t trialheadScore(const HeightMap& map, Position trialheadPos) const
	{
		if (!map.isTrialHead(trialheadPos)) {
			return {};
		}

		std::unordered_set<Position> visited;
		std::stack<Position> stack;
		uint32_t score{};
		
		stack.push(trialheadPos);
		while (!stack.empty()) {
			Position currPos = stack.top();
			stack.pop();

			Vec2D offsets[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
			for (const Vec2D& offset : offsets) {
				auto newPos = currPos + offset;

				if (isStepUp(map, currPos, newPos) && !visited.contains(newPos)) {
					if (map.isFinishPoint(newPos)) {
						visited.insert(newPos);
						score++;
					}
					else {
						stack.push(newPos);
					}
				}
			}

			visited.insert(currPos);
		}

		return score;
	}

	uint32_t trialheadRating(const HeightMap& map, Position trialheadPos) const
	{
		if (!map.isTrialHead(trialheadPos)) {
			return {};
		}

		std::stack<Position> stack;
		uint32_t rating{};

		stack.push(trialheadPos);
		while (!stack.empty()) {
			Position currPos = stack.top();
			stack.pop();

			Vec2D offsets[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
			for (const Vec2D& offset : offsets) {
				auto newPos = currPos + offset;

				if (isStepUp(map, currPos, newPos)) {
					if (map.isFinishPoint(newPos)) {
						rating++;
					}
					else {
						stack.push(newPos);
					}
				}
			}
		}

		return rating;
	}

	bool isStepUp(const HeightMap& hMap, Position from, Position goal) const
	{
		return hMap.at(goal) == hMap.at(from) + 1;
	}
};

int main(int argc, char* args[])
{
	std::vector<std::string> runArgs{ aoc::argsToString(argc - 1, args + 1) };

	if (runArgs.empty()) {
		std::cout << "No arguments given! Running default file." << std::endl;
		runArgs.push_back("input.txt");
	}

	HeihgtMapParser parser;
	HeihgtMapPathFinder finder;
	for (const std::string& arg : runArgs) {
		std::vector<std::string> lines{ aoc::loadFile(arg) };
		HeightMap map = parser.parseHeightMap(lines);
		std::cout << "For file " << arg << std::endl
			<< "Total trialheads score: "
			<< finder.trialheadsScoreSum(map) << std::endl
			<< "Total trialheads rating: "
			<< finder.trialheadsRatingsSum(map) << std::endl;
	}

	return 0;
}