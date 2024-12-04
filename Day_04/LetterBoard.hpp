#pragma once
#include <vector>
#include <string>
#include <stdexcept>

class LetterBoard {
public:
	LetterBoard(const std::vector<std::string>& board)
		: m_board{ board }
	{
		if (!isRectangular())
			throw std::runtime_error("Non rectangle board");
	};

	LetterBoard(std::vector<std::string>&& board)
		: m_board{ std::move(board) }
	{
		if (!isRectangular())
			throw std::runtime_error("Non rectangle board");
	};

	char get(size_t x, size_t y) const
	{
		return m_board[y][x];
	}

	bool contains(size_t x, size_t y) const
	{
		return x >= 0 && x < sizeX() && y >= 0 && y < sizeY();
	}

	size_t sizeX() const
	{
		return m_board[0].size();
	}

	size_t sizeY() const
	{
		return m_board.size();
	}

	int countWordAppearance(const std::string& word, int x, int y) const;
	bool isCrossMas(int x, int y) const;

private:
	enum Dir {
		UP,
		UP_RIGHT,
		RIGHT,
		DOWN_RIGHT,
		DOWN,
		DOWN_LEFT,
		LEFT,
		UP_LEFT
	};
	bool lookForWord(const std::string& word, int x, int y, Dir dir) const;
	bool isRectangular();
	std::vector<std::string> m_board;
};