#include <utils.hpp>
#include <vector>
#include <queue>

template<typename T>
class RectangularBoard
{
public:
	RectangularBoard(size_t sizeX, size_t sizeY)
		: m_fields(sizeY, std::vector<T>(sizeY))
	{}

	RectangularBoard(const std::vector<std::vector<T>>& fields)
		: m_fields{ fields }
	{
		if (!isRectangular()) {
			throw std::invalid_argument("Garden is not rectangular!");
		}
	}

	RectangularBoard(std::vector<std::vector<T>>&& fields)
		: m_fields{ fields }
	{
		if (!isRectangular()) {
			throw std::invalid_argument("Garden is not rectangular!");
		}
	}

	bool contains(const aoc::Position& p) const {
		return p.y >= 0 && p.y < sizeY() && p.x >= 0 && p.x < sizeX();
	}

	T at(const aoc::Position& p) const {
		return m_fields[p.y][p.x];
	}

	void setAt(const aoc::Position& p, T val) {
		if (contains(p)) {
			m_fields[p.y][p.x] = val;
		}
	}

	size_t sizeX() const {
		return m_fields[0].size();
	}

	size_t sizeY() const {
		return m_fields.size();
	}

	void print() const {
		for (const auto& row : m_fields) {
			for (const auto& el : row) {
				std::cout << el;
			}
			std::cout << std::endl;
		}
	}

protected:
	bool isRectangular() {
		if (m_fields.empty()) {
			return false;
		}

		const auto& firstRow = m_fields.front();
		for (const auto& row : m_fields) {
			if (row.size() != firstRow.size()) {
				return false;
			}
		}

		return true;
	}

	std::vector<std::vector<T>> m_fields;
};


class Garden : public RectangularBoard<char>
{
public:
	using RectangularBoard::RectangularBoard;
};


class GardenParser 
{
public:
	Garden parseGarden(const std::vector<std::string>& garden) const
	{
		std::vector<std::vector<char>> fields;
		for (auto& row : garden) {
			fields.push_back({});
			for (auto& fieldName : row) {
				fields.back().push_back(fieldName);
			}
		}
		return Garden{fields};
	}
};


class GardenValueCalculator
{
public:
	uint64_t calculateValue(const Garden& g)
	{
		RectangularBoard<bool> visitedFields(g.sizeX(), g.sizeY());
		uint64_t sum{};

		for (int y = 0; y < visitedFields.sizeY(); ++y) {
			for (int x = 0; x < visitedFields.sizeX(); ++x) {
				if (visitedFields.at({ x, y }) == 0) {
					
					uint64_t area = 1ull;
					uint64_t perimeter{};
					std::queue<aoc::Position> toVisit;
					toVisit.push({x,y});
					while (!toVisit.empty()) {
						aoc::Position currPos = toVisit.front();
						toVisit.pop();
						visitedFields.setAt(currPos, true);

						aoc::Vec2D offsets[]{ {0,-1}, {1, 0}, {0, 1}, {-1,0} };
						for (const auto& offset : offsets) {
							aoc::Position newPos = currPos + offset;
							if (!g.contains(newPos)) {
								++perimeter;
							}
							else if (g.at(newPos) != g.at(currPos)) {
								++perimeter;
							}
							else if(visitedFields.at(newPos) == 0){
								++area;
								toVisit.push(newPos);
								visitedFields.setAt(newPos, true);
							}
						}

					}
					std::cout << g.at({x,y}) << " " << area << " " << perimeter << std::endl;
					sum += area * perimeter;
				}
			}
		}
		return sum;
	}
};


int main(int argc, char* args[])
{
	auto runArgs = aoc::argsToString(argc - 1, args + 1);
	
	if (runArgs.empty()) {
		const std::string defaultFile = "example.txt";
		std::cerr << "No args given, running default file: " << defaultFile;
		runArgs.push_back(defaultFile);
	}
	
	GardenParser parser;
	GardenValueCalculator calculator;
	for (const std::string& arg : runArgs) {
		std::vector<std::string> lines{ aoc::loadFile(arg) };
		Garden garden = parser.parseGarden(lines);
		std::cout << calculator.calculateValue(garden);
	}

	return 0;
}
