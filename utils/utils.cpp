#include "utils.hpp"
#include <fstream>
namespace aoc{
	std::vector<std::string> argsToString(int argc, char* args[])
	{
		std::vector<std::string> parsedArgs(argc);
		for (size_t i = 0; i < argc; i++) {
			parsedArgs[i] = args[i];
		}
		return parsedArgs;
	}
	

	std::vector<std::string> loadFile(const std::string& fileName)
	{
		std::ifstream file(fileName);
		if (!file) {
			throw std::runtime_error("Cannot open file: " + fileName);
		}

		std::vector<std::string> lines;
		for (std::string line; std::getline(file, line);) {
			lines.push_back(line);
		}

		return lines;
	}
	
	uint8_t countDigits(uint64_t num) {
		if (num == 0) {
			return 1;
		}
		
		uint8_t cnt{};
		while(num > 0) {
			++cnt;
			num /= 10;
		}
		
		return cnt;
	}
	
}