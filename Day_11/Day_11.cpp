#include <utils.hpp>
#include <sstream>
#include <unordered_map>

struct Stone
{
	uint64_t number;
	bool operator==(const Stone & s) const {
		return s.number == number;
	}
};

template<>
struct std::hash<Stone>
{
	std::size_t operator() (const Stone& s) const
	{
		return hash<uint64_t>()(s.number);
	}
};

class StonesParser
{
public:
	std::vector<Stone> parseStones(const std::string& line)
	{
		std::vector<Stone> parsedStones;
		std::stringstream ss(line);
		
		uint64_t num;
		while (ss >> num) {
			parsedStones.emplace_back(num);
		}

		if (ss.fail() && !ss.eof()) {
			throw std::runtime_error("Provided input is ill formated!");
		}

		return parsedStones;
	}
};

class StonesBlinker
{
public:
	uint64_t countStonesAfter(const std::vector<Stone>& stones, uint64_t blinkNum)
	{
		uint64_t sum{};
		for (auto& stone : stones) {
			sum += countStones(stone, blinkNum);
		}

		return sum;
	}

private:
	uint64_t countStones(const Stone& stone, uint64_t blinksLeft)
	{
		if (m_mem.contains(stone)) {
			if (m_mem[stone].contains(blinksLeft)) {
				return m_mem[stone][blinksLeft];
			}
		}

		if (blinksLeft == 0) {
			return 1;
		}
		
		uint64_t num = stone.number;
		uint64_t result;
		if (num == 0) {
			result = countStones({ 1 }, blinksLeft - 1);
		}
		else if (aoc::countDigits(num) % 2 == 0) {
			std::string str = std::to_string(num);
			uint64_t numL = std::stoull(str.substr(0, str.length() / 2));
			uint64_t numR = std::stoull(str.substr(str.length() / 2));
			result = countStones({ numL }, blinksLeft - 1)
				+ countStones({ numR }, blinksLeft - 1);
		}
		else {
			result = countStones({num * m_mulContant}, blinksLeft - 1);
		}

		return m_mem[stone][blinksLeft] = result;
	}

	static constexpr uint64_t m_mulContant = 2024;
	static std::unordered_map<Stone, std::unordered_map<uint64_t, uint64_t>> m_mem;
};

std::unordered_map<Stone, std::unordered_map<uint64_t, uint64_t>> StonesBlinker::m_mem;



int main(int argc, char* args[])
{
	std::vector<std::string> runArgs{ aoc::argsToString(argc - 1, args + 1) };

	if (runArgs.empty()) {
		std::cout << "No arguments given! Running default file." << std::endl;
		runArgs.push_back("example.txt");
	}

	StonesParser parser;
	StonesBlinker blinker;
	for (const std::string& arg : runArgs) {
		try {
			std::string line{ aoc::loadFile(arg).at(0) };
			std::vector<Stone> stones = parser.parseStones(line);
			const uint64_t blinkNum = 75;

			std::cout << "In file " << arg << std::endl
				<< "For " << blinkNum << " blinks: " 
				<< blinker.countStonesAfter(stones, 75) << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}
