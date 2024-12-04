#include "utils.hpp"
namespace aoc{
	std::vector<std::string> argsToString(int argc, char* args[])
	{
		std::vector<std::string> parsedArgs(argc);
		for (size_t i = 0; i < argc; i++) {
			parsedArgs[i] = args[i];
		}
		return parsedArgs;
	}
}