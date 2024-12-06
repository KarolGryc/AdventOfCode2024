#include <iostream>
#include <vector>
#include <string>
#include <utils.hpp>

using Board = std::vector<std::string>;

struct Vec2D {
	int x;
	int y;
};

struct Point {
	int x;
	int y;

	bool isInBoard(const Board& b)
	{
		return y >= 0 && y < b.size() && x >= 0 && x < b[y].size();
	}

	int distance(Point p) 
	{
		return std::abs(p.x - this->x) + std::abs(p.x - this->x);
	}

	Point operator-(Vec2D v)
	{
		return {x - v.x, y - v.y};
	}

	Point& operator+=(Vec2D v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
};

class Guard {
public:
	Guard(Point position) : m_pos{ position } {};

	void setPosition(Point position) { m_pos = position; };

	Point findNextPosition(const Board& board)
	{
		Vec2D moveDir{ getDirVector() };
		Point nextPos = m_pos;

		while (nextPos.isInBoard(board) && board[nextPos.y][nextPos.x] != '#') {
			nextPos += moveDir;
		}

		return nextPos - moveDir;
	}

	Point findNextPositionMarkVisited(Board& board)
	{
		Vec2D moveDir{ getDirVector() };
		Point currPos = m_pos;

		while (currPos.isInBoard(board) && board[currPos.y][currPos.x] != '#') {
			board[currPos.y][currPos.x] = 'X';
			currPos += moveDir;
		}

		return currPos - moveDir;
	}

	void rotate90DegRight()
	{
		m_currDir = static_cast<Dir>(((int)m_currDir + 1) % 4);
	}

private:
	Vec2D getDirVector()
	{
		switch (m_currDir) {
			using enum Dir;
			case UP:
				return { 0, -1 };
			case RIGHT: 
				return { 1, 0 };
			case DOWN:	
				return { 0, 1 };
			case LEFT:	
				return { -1, 0 };
			default:	return { 0, 0 };
		}
	}

	enum class Dir {
		UP = 0,
		RIGHT = 1,
		DOWN = 2,
		LEFT = 3
	};

	Point m_pos;
	Dir m_currDir = Dir::UP;
};

Point findGuardPos(const Board& board) 
{
	for (int y = 0; y < board.size(); y++) {
		for (int x = 0; x < board[y].size(); x++) {
			if (board[y][x] == '^')
				return { x, y };
		}
	}
	return { -1, -1 };
}

uint64_t countVisitedFields(Guard guard, Board board)
{
	while (guard.findNextPositionMarkVisited(board).isInBoard(board)) {
		guard.rotate90DegRight();
	}

	uint64_t cnt{};
	for (auto& row : board) {
		for (auto c : row) {
			if (c == 'X') ++cnt;
		}
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
			Board board = aoc::loadFile(arg);
			Guard guard(findGuardPos(board));
			std::cout << "Fields visited in " << arg << ": " 
						<< countVisitedFields(guard, board);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
	return 0;
}
