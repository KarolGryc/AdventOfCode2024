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

	void print() const 
	{
		for (const auto& row : m_fields) {
			for (const auto& el : row) {
				std::cout << el;
			}
			std::cout << std::endl;
		}
	}

protected:
	bool isRectangular() 
	{
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


class GardenArea
{
public:
	GardenArea() 
		: m_name{}, m_fields{} 
	{}

	GardenArea(char name, const std::unordered_set<aoc::Position>& fields)
		: m_name{ name }, m_fields{ fields }
	{}

	GardenArea(char name, std::unordered_set<aoc::Position>&& fields)
		: m_name{name}, m_fields {std::move(fields)}
	{}

	char getName() const {
		return m_name;
	}

	const std::unordered_set<aoc::Position>& getFieldPositions() const {
		return m_fields;
	}

	std::unordered_set<aoc::Position> getBorderFields() const 
	{
		std::unordered_set<aoc::Position> borderPositions;
		for (const auto& field : m_fields) {
			if (isBorder(field)) {
				borderPositions.insert(field);
			}
		}
		return borderPositions;
	}

	uint64_t getPerimeter() const {
		uint64_t perimeter{};
		auto borderFields = getBorderFields();
		for (auto& field : borderFields) {
			perimeter += countEdges(field);
		}

		return perimeter;
	}

	uint64_t getArea() const {
		return m_fields.size();
	}

	uint64_t getSidesCount() const {
		uint64_t sides{};
		for (auto& field : m_fields) {
			sides += countCorners(field);
		}
		return sides;
	}

private:
	bool isBorder(const aoc::Position& fieldPos) const 
	{
		aoc::Vec2D offsets[]{ {0,-1}, {1, 0}, {0, 1}, {-1,0} };
		for (const auto& offset : offsets) {
			if (!m_fields.contains(fieldPos + offset)) {
				return true;
			}
		}

		return false;
	}

	uint8_t countEdges(const aoc::Position& fieldPos) const {
		uint8_t count{};
		aoc::Vec2D offsets[]{ {0,-1}, {1, 0}, {0, 1}, {-1,0} };
		for (const auto& offset : offsets) {
			if (!m_fields.contains(fieldPos + offset)) {
				++count;
			}
		}

		return count;
	}

	uint8_t countCorners(const aoc::Position& fieldPos) const {
		uint8_t count{};
		aoc::Vec2D offsetsX[] = { {-1, 0}, {1, 0}};
		aoc::Vec2D offsetsY[] = { {0, -1}, {0, 1} };
		for (auto& offsetY : offsetsY) {
			for (auto& offsetX : offsetsX) {
				aoc::Position horizontal = fieldPos + offsetX;
				aoc::Position vertical = fieldPos + offsetY;
				bool containsHorizontal = m_fields.contains(horizontal);
				bool containsVertical = m_fields.contains(vertical);
				if (containsHorizontal == false && containsVertical == false) {
					count++;
				}
				else if (containsHorizontal == true && containsVertical == true) {
					count += !m_fields.contains(fieldPos + offsetX + offsetY);
				}
			}
		}
		return count;
	}

	char m_name;
	std::unordered_set<aoc::Position> m_fields;
};


class Garden : public RectangularBoard<char>
{
public:
	using RectangularBoard::RectangularBoard;

	GardenArea getAreaAt(const aoc::Position& pos) const 
	{
		if (!contains(pos)) {
			return {};
		}

		std::unordered_set<aoc::Position> areaPositions{pos};
		std::queue<aoc::Position> toVisit{ {pos} };
		while (!toVisit.empty()) {
			aoc::Position currPos = toVisit.front();
			toVisit.pop();

			auto neighbours = getValidNeighbors(currPos);

			for (const auto& neighbour : neighbours) {
				if (!areaPositions.contains(neighbour)) {
					areaPositions.insert(neighbour);
					toVisit.push(neighbour);
				}
			}
		}

		char areaName = at(pos);
		return GardenArea{ areaName, areaPositions };
	}

	std::vector<GardenArea> getAllAreas() const 
	{
		RectangularBoard<bool> visited(sizeX(), sizeY());
		std::vector<GardenArea> gardenAreas;

		for (int y = 0; y < visited.sizeY(); ++y) {
			for (int x = 0; x < visited.sizeX(); ++x) {
				aoc::Position pos{ x,y };
				if (visited.at(pos) == 1) {
					continue;
				}

				GardenArea area = getAreaAt(pos);
				gardenAreas.push_back(area);
				for (const auto& position : area.getFieldPositions()) {
					visited.setAt(position, true);
				}
			}
		}

		return gardenAreas;
	}

private:
	std::unordered_set<aoc::Position> getValidNeighbors(
		const aoc::Position& position) const
	{
		auto neighbours = getNeighbours(position);
		std::unordered_set<aoc::Position> validNeighbours;

		for (const auto& neighbour : neighbours) {
			if (isValidNeighbour(position, neighbour)) {
				validNeighbours.insert(neighbour);
			}
		}

		return validNeighbours;
	}

	std::unordered_set<aoc::Position> getNeighbours(const aoc::Position& pos) const
	{
		std::unordered_set<aoc::Position> neighbours;
		aoc::Vec2D offsets[]{ {0,-1}, {1, 0}, {0, 1}, {-1,0} };
		for (const auto& offset : offsets) {
			aoc::Position nPos = pos + offset;
			neighbours.insert(nPos);
		}

		return neighbours;
	}

	bool isValidNeighbour(
		const aoc::Position pos, 
		const aoc::Position& neigh) const 
	{
		return contains(pos) && contains(neigh) && at(pos) == at(neigh);
	}
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
	uint64_t calculateValue(const Garden& g) const
	{
		uint64_t sum{};
		std::vector<GardenArea> areas = g.getAllAreas();
		for (const auto& area : areas) {
			sum += area.getArea() * area.getPerimeter();
		}
		return sum;
	}

	uint64_t calculateDiscountedValue(const Garden& g) const
	{
		uint64_t sum{};
		std::vector<GardenArea> areas = g.getAllAreas();
		for (const auto& area : areas) {
			sum += area.getArea() * area.getSidesCount();
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
		std::cout << "Normal value: "
			<< calculator.calculateValue(garden)
			<< std::endl
			<< "Discounted value: "
			<< calculator.calculateDiscountedValue(garden)
			<< std::endl;
	}

	return 0;
}
