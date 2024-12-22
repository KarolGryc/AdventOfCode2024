#include <utils.hpp>
#include <regex>
#include <sstream>

class NumParser
{
public:
	std::vector<uint64_t> parseNums(const std::vector<std::string>& lines)
	{
		std::vector<uint64_t> nums;
		for (auto& line : lines) {
			std::stringstream ss(line);
			uint64_t num;
			ss >> num;
			if (ss.fail()) {
				throw std::invalid_argument("Invalid number format.");
			}
			nums.push_back(num);
		}

		return nums;
	}
};

class MonkeysSolution
{
public:
	uint64_t sumSecretNums(const std::vector<uint64_t>& nums, uint64_t numOfIterations) const
	{
		uint64_t sum{};
		for (auto& num : nums) {
			sum += getSecretNumAfter(num, numOfIterations);
		}
		
		return sum;
	}

	uint64_t getSecretNumAfter(uint64_t num, uint64_t numOfIterations) const
	{
		for (uint64_t i = 0; i < numOfIterations; ++i) {
			num = getNewSecretNum(num);
		}

		return num;
	}

	uint64_t getNewSecretNum(uint64_t num) const
	{
		// step 1 multiply by 64
		num = mix(num, num << 6);
		num = prune(num);

		// step 2 divide by 32
		num = mix(num, num >> 5);
		num = prune(num);

		// step 3 multiply by 2048
		num = mix(num, num << 11);
		num = prune(num);

		return num;
	}

private:
	uint64_t prune(uint64_t oldSecretNum) const
	{
		// modulo 16777216
		static constexpr uint64_t pruneConst = 16777216 - 1;
		return oldSecretNum & (pruneConst);
	}

	uint64_t mix(uint64_t oldSecretNum, uint64_t mixedNum) const
	{
		return oldSecretNum ^ mixedNum;
	}
};

int main(int argc, char* args[])
{
	auto runArgs = aoc::argsToString(argc - 1, args + 1);
	if (runArgs.empty()) {
		const std::string defaultFile = "example.txt";
		std::cerr << "No args given, running default file: " 
			<< defaultFile 
			<< std::endl;
		runArgs.push_back(defaultFile);
	}
	
	NumParser parser;
	MonkeysSolution solution;
	for (const std::string& arg : runArgs) {
		try {
			static constexpr uint64_t numOfIterations = 2000;
			auto lines = aoc::loadFile(arg);
			auto nums = parser.parseNums(lines);
			std::cout << "Sum of secret nums after " << numOfIterations << ": "
				<< solution.sumSecretNums(nums, numOfIterations) << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}