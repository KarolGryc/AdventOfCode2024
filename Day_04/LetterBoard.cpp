#include "LetterBoard.hpp"
#include <iostream>

bool LetterBoard::isRectangular()
{
	for (const auto& row : m_board) {
		if (row.size() != sizeX()) {
			return false;
		}
	}
	return true;
}

bool LetterBoard::lookForWord(const std::string& word, int x, int y, Dir dir) const
{
	if (!contains(x, y)) {
		return false;
	}

	if (get(x, y) == word[0]) {
		if (word.size() == 1) {
			return true;
		}
	}
	else {
		return false;
	}

	std::string restOfWord = word.substr(1);
	switch (dir) {
		using enum Dir;
		case UP:
			return lookForWord(restOfWord, x, y - 1, UP);
			break;
		case UP_RIGHT:
			return lookForWord(restOfWord, x + 1, y - 1, UP_RIGHT);
			break;
		case RIGHT:
			return lookForWord(restOfWord, x + 1, y, RIGHT);
			break;
		case DOWN_RIGHT:
			return lookForWord(restOfWord, x + 1, y + 1, DOWN_RIGHT);
			break;
		case DOWN:
			return lookForWord(restOfWord, x, y + 1, DOWN);
			break;
		case DOWN_LEFT:
			return lookForWord(restOfWord, x - 1, y + 1, DOWN_LEFT);
			break;
		case LEFT:
			return lookForWord(restOfWord, x - 1, y, LEFT);
			break;
		case UP_LEFT:
			return lookForWord(restOfWord, x - 1, y - 1, UP_LEFT);
			break;
	}
}

int LetterBoard::countWordAppearance(const std::string& word, int x, int y) const
{
	int sum{};
	if (contains(x, y) && word[0] == get(x, y)) {
		using enum Dir;
		std::string restOfWord = word.substr(1);
		sum += lookForWord(restOfWord, x, y - 1, UP);
		sum += lookForWord(restOfWord, x + 1, y - 1, UP_RIGHT);
		sum += lookForWord(restOfWord, x + 1, y, RIGHT);
		sum += lookForWord(restOfWord, x + 1, y + 1, DOWN_RIGHT);
		sum += lookForWord(restOfWord, x, y + 1, DOWN);
		sum += lookForWord(restOfWord, x - 1, y + 1, DOWN_LEFT);
		sum += lookForWord(restOfWord, x - 1, y, LEFT);
		sum += lookForWord(restOfWord, x - 1, y - 1, UP_LEFT);
	}
	return sum;
}


bool LetterBoard::isCrossMas(int x, int y) const
{
	if (x >= 1 && x < sizeX() - 1 && y >= 1 && y <= sizeY() - 1) {
		return	lookForWord("MAS", x - 1, y - 1, Dir::DOWN_RIGHT) +
			lookForWord("MAS", x + 1, y - 1, Dir::DOWN_LEFT) +
			lookForWord("MAS", x - 1, y + 1, Dir::UP_RIGHT) +
			lookForWord("MAS", x + 1, y + 1, Dir::UP_LEFT) == 2;
	}
	return false;
}