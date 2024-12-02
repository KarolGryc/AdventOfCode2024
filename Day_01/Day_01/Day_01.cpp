#include <utility>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <execution>
#include <unordered_map>
#include <numeric>

std::vector<std::string> argsToString(int argc, char* args[]);

using intVecPair = std::pair<std::vector<int>, std::vector<int>>;
intVecPair parseListsFromFile(const std::string& fileName);
uint64_t distancesSum(const intVecPair& inputLists);
uint64_t similaritiesSum(const intVecPair& inputLists);

int main(int argc, char* args[])
{
	std::vector<std::string> runtimeArguments(argsToString(argc - 1, args + 1));
	
	if (runtimeArguments.empty()) {
		std::cout << "No runtime arguments!";
		return -1;
	}

	for (const std::string& argument : runtimeArguments) {
		 intVecPair parsedPair{ parseListsFromFile(argument) };
		 uint64_t result = distancesSum(parsedPair);
		 std::cout << "Distances from file " << argument 
				   << ": " << result << std::endl;

		 uint64_t simmilaritiesResult = similaritiesSum(parsedPair);
		 std::cout << "Similarities from file " << argument 
				   << ": " << simmilaritiesResult << std::endl;
	}

	return 0;
}



std::vector<std::string> argsToString(int argc, char* args[]) 
{
	std::vector<std::string> parsedArgs(argc);
	for (int i = 0; i < argc; i++) {
		parsedArgs[i] = args[i];
	}
	return parsedArgs;
}


std::pair<std::vector<int>, std::vector<int>> parseListsFromFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file) {
		throw std::runtime_error("Cannot open input file!");
	}

	std::vector<int> firstList;
	std::vector<int> secondList;
	std::string line;
	while (std::getline(file, line)) {
		int firstElement;
		int secondElement;
		std::istringstream ss(line);
		ss >> firstElement >> secondElement;

		if (ss.fail()) {
			throw std::runtime_error("File: " + fileName + " is ill-formed!");
		}

		firstList.push_back(firstElement);
		secondList.push_back(secondElement);
	}

	return { firstList, secondList };
}


uint64_t distancesSum(const intVecPair& inputLists) 
{
	std::vector<int> firstVec = inputLists.first;
	std::vector<int> secondVec = inputLists.second;

	if (firstVec.size() != secondVec.size()) {
		throw std::invalid_argument("Input lists have different sizes!");
	}

	std::sort(std::execution::par, firstVec.begin(), firstVec.end(), std::less{});
	std::sort(std::execution::par, secondVec.begin(), secondVec.end(), std::less{});

	uint64_t sum = 0;
	size_t n = firstVec.size();
	for (int i = 0; i < n; ++i) {
		sum += std::abs(firstVec[i] - secondVec[i]);
	}
	
	return sum;
}


uint64_t similaritiesSum(const intVecPair& inputLists)
{
	std::unordered_map<int, uint64_t> elementCount;
	for (auto el : inputLists.second) {
		++elementCount[el];
	}

	return std::accumulate(inputLists.first.begin(), inputLists.first.end(), 0ull,
		[&elementCount](uint64_t sum, int el) {
			return elementCount.contains(el) ? sum + el * elementCount[el] : sum;
		}
	);
}