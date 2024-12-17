#include <utils.hpp>
#include <memory>

// ugh, solution straight from nightmare

class BoardElement
{
public:
	virtual bool isMovable() const = 0;
};

class Wall : public BoardElement
{
public:
	bool isMovable() const {
		return false;
	}
};

class Box : public BoardElement
{
public:
	bool isMovable() const {
		return true;
	}
};

class BoxL : public Box
{};

class BoxR : public Box
{};

class Warehouse
{
public:
	Warehouse(std::vector<std::vector<std::shared_ptr<BoardElement>>>&& tiles, aoc::Position robotPos)
		: m_board{ std::move(tiles) }, m_robotPos(robotPos)
	{}

	size_t sizeX() const {
		return m_board[0].size();
	}

	size_t sizeY() const {
		return m_board.size();
	}

	bool contains(const aoc::Position& p) const {
		return p.x >= 0 && static_cast<size_t>(p.x) < sizeX() 
			&& p.y >= 0 && static_cast<size_t>(p.y) < sizeY();
	}

	std::shared_ptr<BoardElement>& at(const aoc::Position& p) {
		return m_board[p.y][p.x];
	}

	const std::shared_ptr<BoardElement>& at(const aoc::Position& p) const {
		return m_board[p.y][p.x];
	}

	void display() const {
		for (int y = 0; y < sizeY(); ++y) {
			const auto& row = m_board[y];
			for (int x = 0; x < sizeX(); ++x) {
				if (aoc::Position{ x,y } == m_robotPos) {
					std::cout << "@ ";
					continue;
				}

				const auto& element = row[x];
				if (element) {
					if (!element->isMovable()) {
						std::cout << "# ";
					}
					else if (std::dynamic_pointer_cast<BoxL>(element)) {
						std::cout << "[ ";
					}
					else if (std::dynamic_pointer_cast<BoxR>(element)) {
						std::cout << "] ";
					}
					else if (std::dynamic_pointer_cast<Box>(element)) {
						std::cout << "O ";
					}
					else {
						std::cout << "? ";
					}
				}
				else {
					std::cout << ". ";
				}
			}
			std::cout << std::endl;
		}
	}

	void performMoves(const std::vector<aoc::Vec2D>& moves) {
		for (const aoc::Vec2D& move : moves)
		{
			aoc::Position nextPos = m_robotPos + move;
			auto& nextField = at(nextPos);
			if (nextField == nullptr) {
				m_robotPos = nextPos;
			}
			else {
				if (isMovePossible(nextPos, move)) {
					moveWithPush(nextPos, move);
					m_robotPos = nextPos;
				}
			}
		}
	}

	uint64_t sumOfCoordinatesGPS() const {
		uint64_t sum{};
		for (int64_t y = 0; y < (int64_t)sizeY(); ++y) {
			for (int64_t x = 0; x < (int64_t)sizeX(); ++x) {
				auto& el = at({ x,y });
				if (el != nullptr && el->isMovable()) {
					if (!std::dynamic_pointer_cast<BoxR>(el)) {
						sum += y * 100 + x;
					}
				}
			}
		}
		return sum;
	}

private:
	bool isMovePossible(aoc::Position pos, aoc::Vec2D v)
	{
		aoc::Position nextField = pos + v;
		if (!contains(pos)) {
			return false;
		}

		// RTTI horror x_x
		std::shared_ptr<BoardElement> el = at(pos);
		if (el == nullptr) {
			return true;
		}
		else if (el->isMovable()) {
			bool isNeighbourValid = true;
			if (v == aoc::Vec2D{ 0,1 } || v == aoc::Vec2D{0, -1}) {
				if (std::dynamic_pointer_cast<BoxL>(el)) {
					aoc::Position nextForRight = nextField + aoc::Vec2D{ 1,0 };
					isNeighbourValid = isMovePossible(nextForRight, v);
				}
				else if (std::dynamic_pointer_cast<BoxR>(el)) {
					aoc::Position nextForLeft = nextField + aoc::Vec2D{ -1,0 };
					isNeighbourValid = isMovePossible(nextForLeft, v);
				}
			}
			return isNeighbourValid && isMovePossible(nextField, v);
		}
		else {
			return false;
		}
	}

	void moveWithPush(aoc::Position pos, aoc::Vec2D v, std::unordered_set<aoc::Position> visited = {})
	{
		if (visited.contains(pos)) {
			return;
		}
		visited.insert(pos);

		// RTTI horror x_x
		std::shared_ptr<BoardElement> curr = at(pos);
		if (std::dynamic_pointer_cast<BoxL>(curr)) {
			moveWithPush(pos + aoc::Vec2D{ 1, 0}, v, visited);
		}
		else if (std::dynamic_pointer_cast<BoxR>(curr)) {
			moveWithPush(pos + aoc::Vec2D{ -1, 0 }, v, visited);
		}

		aoc::Position nextField = pos + v;
		if (at(nextField) != nullptr) {
			if (at(nextField)->isMovable()) {
				moveWithPush(nextField, v, visited);
			}
		}

		std::swap(at(pos), at(nextField));
	}

	aoc::Position m_robotPos;
	std::vector<std::vector<std::shared_ptr<BoardElement>>> m_board;
};


class WarehouseParser
{
public:
	Warehouse parseWarehouse(const std::vector<std::string>& lines)
	{
		std::vector<std::vector<std::shared_ptr<BoardElement>>> parsedTiles;
		aoc::Position robotInitPos{};

		for (int y = 0; y < lines.size(); y++) {
			const std::string& line = lines[y];

			std::vector<std::shared_ptr<BoardElement>> parsedRow;
			for (int x = 0; x < line.size(); x++) {
				char c = line[x];
				std::shared_ptr<BoardElement> parsedElement = nullptr;
				if (c == wall) parsedElement = std::make_shared<Wall>();
				else if (c == box) parsedElement = std::make_shared<Box>();
				else if (c == robot) robotInitPos = { x, y };
				parsedRow.push_back(parsedElement);
			}

			parsedTiles.push_back(std::move(parsedRow));
		}

		return Warehouse{ std::move(parsedTiles), robotInitPos };
	}

	Warehouse parseWideWarehouse(const std::vector<std::string>& lines)
	{
		std::vector<std::vector<std::shared_ptr<BoardElement>>> parsedTiles;
		aoc::Position robotInitPos{};

		for (int y = 0; y < lines.size(); y++) {
			const std::string& line = lines[y];

			std::vector<std::shared_ptr<BoardElement>> parsedRow;
			for (int x = 0; x < line.size(); x++) {
				char c = line[x];
				if (c == wall) {
					parsedRow.push_back(std::make_shared<Wall>());
					parsedRow.push_back(std::make_shared<Wall>());
				}
				else if (c == box) {
					parsedRow.push_back(std::make_shared<BoxL>());
					parsedRow.push_back(std::make_shared<BoxR>());
				}
				else {
					if (c == robot) {
						int64_t robotX = static_cast<int64_t>(parsedRow.size());
						int64_t robotY = y;
						robotInitPos = { robotX, robotY };
					}
					parsedRow.push_back(nullptr);
					parsedRow.push_back(nullptr);
				}
			}

			parsedTiles.push_back(std::move(parsedRow));
		}

		return Warehouse{ std::move(parsedTiles), robotInitPos };
	}

	std::vector<aoc::Vec2D> parseMoves(const std::string& line)
	{
		std::vector<aoc::Vec2D> result;
		for (auto c : line) {
			aoc::Vec2D dir;
			if (c == up) dir = { 0,-1 };
			else if (c == left) dir = { -1,0 };
			else if (c == down) dir = { 0,1 };
			else if (c == right) dir = { 1,0 };
			else dir = { 0,0 };

			result.push_back(dir);
		}

		return result;
	}

	auto splitData(const std::vector<std::string>& lines)
		-> std::pair<std::vector<std::string>, std::string>
	{
		std::pair<std::vector<std::string>, std::string> result;
		for (auto& line : lines) {
			if (line.empty()) {
				break;
			}
			result.first.push_back(line);
		}

		const std::string& lastLine = lines.back();
		size_t firstNotOf = lastLine.find_first_not_of({ up,left,down,right });
		if (firstNotOf == std::string::npos) {
			result.second = lastLine;
		}
		else {
			throw std::exception("Incorrect move data format!");
		}

		return result;
	}

private:
	static constexpr char up = '^';
	static constexpr char left = '<';
	static constexpr char down = 'v';
	static constexpr char right = '>';
	static constexpr char wall = '#';
	static constexpr char box = 'O';
	static constexpr char empty = '.';
	static constexpr char robot = '@';
};

int main(int argc, char* args[])
{
	auto runArgs = aoc::argsToString(argc - 1, args + 1);

	if (runArgs.empty()) {
		const std::string defaultFile = "picoExample.txt";
		std::cerr << "No args given, running default file: " << defaultFile;
		runArgs.push_back(defaultFile);
	}

	WarehouseParser parser;
	for (const std::string& arg : runArgs) {
		try {
			// load data from file
			std::vector<std::string> lines{ aoc::loadFile(arg) };
			auto [warehouseLines, movesLines] = parser.splitData(lines);
			auto warehouse = parser.parseWarehouse(warehouseLines);
			auto moves = parser.parseMoves(movesLines);

			// GPS coords for normal warehouse
			warehouse.performMoves(moves);
			std::cout << "Sum of GPS coord: " << warehouse.sumOfCoordinatesGPS() << std::endl;

			// GPS coords for wide warehouse
			auto wideWarehouse = parser.parseWideWarehouse(warehouseLines);
			wideWarehouse.performMoves(moves);
			std::cout << "Sum of GPS coord for wide boxes: " 
				<< wideWarehouse.sumOfCoordinatesGPS() << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}