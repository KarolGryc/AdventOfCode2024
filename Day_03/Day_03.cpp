#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>

// general utils
std::string loadTextFile(const std::string& fileName);
std::vector<std::string> argsToString(int argc, char* args[]);
std::vector<std::smatch> findAllExpressions(const std::string& line, const std::regex& expression);

// solution
int evalMul(const std::string& expression);
std::vector<std::string> findMuls(const std::string& fileContent);
std::vector<std::string> findInstructions(const std::string& code);
uint64_t sumInstructions(const std::vector<std::string>& instructions);


int main(int argc, char* args[])
{
	std::vector<std::string> runtimeArgs{ argsToString(argc - 1, args + 1) };
	if (runtimeArgs.empty()) {
		std::cerr << "No file names given!" << std::endl;
		return 1;
	}


	for (const std::string& arg : runtimeArgs) {
		try {
			std::string fileContent{ loadTextFile(arg) };
			std::cout	<< "Sum of just muls from file: " << arg 
						<< " " << sumInstructions(findMuls(fileContent)) 
						<< std::endl;

			std::cout	<< "Sum of muls with do's and dont's in file: " << arg
						<< " " << sumInstructions(findInstructions(fileContent)) 
						<< std::endl;
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}
	}

	return 0;
}



std::string loadTextFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file) {
		throw std::runtime_error("Cannot open file: " + fileName);
	}

	std::string fileContent;
	for (std::string line; std::getline(file, line);) {
		fileContent += line;
	}

	return fileContent;
}


std::vector<std::smatch> findAllExpressions(const std::string& line, const std::regex& exp)
{
	std::vector<std::smatch> matches;
	auto begin = std::sregex_iterator(line.begin(), line.end(), exp);
	auto end = std::sregex_iterator();

	for (auto& it = begin; it != end; it++) {
		matches.push_back(*it);
	}

	return matches;
}


std::vector<std::string> findInstructions(const std::string& code)
{
	std::vector<std::string> vec;
	std::regex reg(R"RAW(do(?:n't)?\(\)|mul\(\d{1,3},\d{1,3}\))RAW");
	auto matches = findAllExpressions(code, reg);

	for (const auto& match : matches)
	{
		vec.emplace_back(match.str());
	}

	return vec;
}


std::vector<std::string> findMuls(const std::string& code) 
{
	std::vector<std::string> vec;
	std::regex reg(R"RAW(mul\(\d{1,3},\d{1,3}\))RAW");
	auto mulMatches = findAllExpressions(code, reg);

	for (const auto& mulMatch : mulMatches)
	{
		vec.emplace_back(mulMatch.str());
	}

	return vec;
}

uint64_t sumInstructions(const std::vector<std::string>& instructions)
{
	uint64_t sum{};
	bool add = true;
	for (const auto& instruction : instructions) {
		if (instruction == "do()") {
			add = true;
		}
		else if (instruction == "don't()") {
			add = false;
		}
		else if (add) {
			sum += evalMul(instruction);
		}
	}
	return sum;
}

int evalMul(const std::string& mul)
{
	auto matches = findAllExpressions(mul, std::regex(R"RAW(\d{1,})RAW"));
	if (matches.size() != 2) {
		throw std::invalid_argument("mul is ill formed");
	}

	int firstNum	= std::stoi(matches[0].str());
	int secondNum	= std::stoi(matches[1].str());
	return firstNum * secondNum;
}


std::vector<std::string> argsToString(int argc, char* args[])
{
	std::vector<std::string> parsedArgs(argc);
	for (size_t i = 0; i < argc; i++) {
		parsedArgs[i] = args[i];
	}
	return parsedArgs;
}