#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "utils.hpp"
#include "LetterBoard.hpp"

std::vector<std::string> loadFile(const std::string& fileName);

uint64_t countXmasAppearance(const LetterBoard& board);
uint64_t countCrossMasAppeareance(const LetterBoard& board);

int main(int argc, char* args[])
{
	std::vector<std::string> runArgs{ aoc::argsToString(argc - 1, args + 1) };
	
	if (runArgs.empty()) {
		std::cerr << "No files to check!" << std::endl;
		return 1;
	}

	for (const std::string& arg : runArgs) {
		try {
			LetterBoard board{ loadFile(arg) };
			std::cout << "Num of XMAS for file: " << arg
				<< " " << countXmasAppearance(board) << std::endl;
			std::cout << "Num of X-MAS for file: " << arg
				<< " " << countCrossMasAppeareance(board) << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	return 0;
}


uint64_t countXmasAppearance(const LetterBoard& board)
{
	uint64_t cnt{};
	for (int y = 0; y < board.sizeY(); y++) {
		for (int x = 0; x < board.sizeX(); x++) {
			std::string word = "XMAS";
			cnt += board.countWordAppearance(word, x, y);
		}
	}
	return cnt;
}


uint64_t countCrossMasAppeareance(const LetterBoard& board)
{
	uint64_t cnt{};
	for (int y = 1; y < board.sizeY() - 1; y++) {
		for (int x = 1; x < board.sizeX() - 1; x++) {
			cnt += board.isCrossMas(x, y);
		}
	}
	return cnt;
}


std::vector<std::string> loadFile(const std::string& fileName)
{
	std::ifstream file(fileName);

	if (!file) {
		throw std::runtime_error("Can't open file: " + fileName);
	}

	std::vector<std::string> fileContent;
	while(!file.eof()) {
		std::string line;
		std::getline(file, line);
		fileContent.emplace_back(line);
	}

	return fileContent;
}