#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <utils.hpp>


struct Vec2D 
{
	int x, y;
};


struct Point 
{
	int x, y;

	bool operator==(const Point& p) const { return x == p.x && y == p.y; }
	Point operator+(Vec2D v) const { return{ x + v.x, y + v.y }; }
	Point operator-(Vec2D v) const { return {x - v.x, y - v.y}; }
	Point& operator+=(Vec2D v) { return *this = *this + v; }
};

template<>
struct std::hash<Point>
{
	size_t operator()(const Point& p) const noexcept
	{
		return std::hash<int>{}(p.x) ^ std::hash<int>{}(p.y) << 1;
	}
};


class Guard 
{
public:
	enum class Dir {
		UP = 0,
		RIGHT = 1,
		DOWN = 2,
		LEFT = 3
	};

	Guard(Point pos = {}, Dir dir = Dir::UP) : m_pos{pos}, m_dir{dir} {};
	Point getPosition() const { return m_pos; }
	Point getNextStep() const { return  m_pos + getDirVector(); }

	void moveTo(Point position) { m_pos = position; };
	void turnRight() { m_dir = static_cast<Dir>(((int)m_dir + 1) % 4); }
	bool facesUp() { return m_dir == Dir::UP; }

private:
	Vec2D getDirVector() const
	{
		switch (m_dir) {
			using enum Dir;
			case UP:	return { 0,-1 };
			case RIGHT: return { 1, 0 };
			case DOWN:	return { 0, 1 };
			case LEFT:	return {-1, 0 };
			default:	return { 0, 0 };
		}
	}

	Dir m_dir = Dir::UP;
	Point m_pos;
};

class Board
{
public:
	Board(const std::vector<std::string>& board)
		:	m_board{ board }, 
			m_initialGuardPos{ findGuardOnBoard() } 
	{}

	Board(std::vector<std::string>&& board)
		:	m_board{ std::move(board) }, 
			m_initialGuardPos{ findGuardOnBoard() } 
	{}

	bool contains(const Point& p) const
	{
		return p.y >= 0 && p.y < sizeY() && p.x >= 0 && p.x < sizeX(p.y);
	}

	void addObstacle(const Point& p)
	{
		m_board[p.y][p.x] = m_obstacleChar;
	}

	void removeObstacle(const Point& p)
	{
		m_board[p.y][p.x] = m_defaultChar;
	}

	std::unordered_set<Point> getVisitedFields() const
	{
		Guard guard(m_initialGuardPos);
		std::unordered_set<Point> visitedTiles{ m_initialGuardPos };
		Point nextTile = guard.getNextStep();
		while (contains(nextTile)) {
			if (isObstacle(nextTile)) {
				guard.turnRight();
			}
			else {
				guard.moveTo(nextTile);
				visitedTiles.insert(nextTile);
			}
			nextTile = guard.getNextStep();
		}

		return visitedTiles;
	}

	bool isGuardPathLooped() const
	{
		std::unordered_set<Point> visitedUpTiles{m_initialGuardPos};
		Guard guard(m_initialGuardPos);
		Point nextStep = guard.getNextStep();

		while (contains(nextStep)) {
			if (isObstacle(nextStep)) {
				if (guard.facesUp()) {
					if (visitedUpTiles.contains(nextStep)) {
						return true;
					}
					visitedUpTiles.insert(nextStep);
				}
				guard.turnRight();
			}
			else {
				guard.moveTo(nextStep);
			}

			nextStep = guard.getNextStep();
		}

		return false;
	}

	Point getInitialGuardPosition() const
	{
		return m_initialGuardPos;
	}

	size_t sizeY() const
	{
		return m_board.size();
	}

	size_t sizeX(size_t row) const
	{
		return m_board[row].size();
	}

private:
	bool isObstacle(const Point& p) const
	{
		return m_board[p.y][p.x] == m_obstacleChar;
	}

	Point findGuardOnBoard()
	{
		for (int y = 0; y < sizeY(); y++) {
			for (int x = 0; x < sizeX(y); x++) {
				if (m_board[y][x] == m_guardChar)
					return { x,y };
			}
		}
		throw std::runtime_error("No guard on given board!");
	}

	static constexpr const char m_guardChar = '^';
	static constexpr const char m_obstacleChar = '#';
	static constexpr const char m_defaultChar = '.';

	std::vector<std::string> m_board;
	Point m_initialGuardPos;
};

uint64_t countVisitedFields(const Board& board)
{
	return board.getVisitedFields().size();
}

uint64_t countLoopingObstacles(Board& board) 
{
	auto visitedFields = board.getVisitedFields();
	visitedFields.erase(board.getInitialGuardPosition());

	uint64_t cnt{};
	for (const Point& field : visitedFields) {
		board.addObstacle(field);
		cnt += board.isGuardPathLooped();
		board.removeObstacle(field);
	}

	return cnt;
}


int main(int argc, char* args[])
{
	auto runtimeArgs{ aoc::argsToString(argc - 1, args + 1) };
	if (runtimeArgs.empty()) {
		std::cerr << "No given files to analyse!" << std::endl;
		return 1;
	}

	for (const std::string& arg : runtimeArgs) {
		try {
			Board board{ aoc::loadFile(arg) };
			std::cout
				<< "File: " << arg << std::endl
				<< "Number of visited fields: "
				<< countVisitedFields(board) << std::endl
				<< "Number of looping obstacles: "
				<< countLoopingObstacles(board) << std::endl;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	return 0;
}