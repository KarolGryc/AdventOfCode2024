#include <utils.hpp>
#include <regex>
#include <sstream>
#include <bit>
#include <unordered_map>

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

class Sequence
{
public:
	Sequence() = default;

	bool operator==(const Sequence& seq) const
	{
		return m_num == seq.m_num;
	}

	void pushNum(uint8_t num)
	{
		static constexpr uint8_t bitsInByte = 8;
		m_num <<= bitsInByte;
		m_num |= static_cast<uint8_t>(num);
	}


	// 0 is newest, 3 is oldest
	int8_t getNumInSeq(uint8_t index) const
	{
		if constexpr (std::endian::native == std::endian::little) {
			return m_numBytes[index];
		}
		else if constexpr (std::endian::native == std::endian::big) {
			return m_numBytes[3 - index];
		}
		else {
			static_assert("Mixed endian?");
		}
	}

	uint32_t getHash() const
	{
		return m_num;
	}

private:
	union
	{
		uint32_t m_num;
		int8_t m_numBytes[4];
	};
};

template <> 
struct std::hash<Sequence>
{
	size_t operator()(const Sequence& sequence) const
	{
		return sequence.getHash();
	}
};

class MonkeysSolution
{
public:
	uint64_t maxBananasNum(const std::vector<uint64_t>& initialNums, uint64_t itNum) const
	{
		std::unordered_map<Sequence, uint64_t> bananasPerSequenceSum;
		for (auto num : initialNums) {
			auto&& sequencesToBananas = possibleBananas(num, itNum);
			for (auto& el : sequencesToBananas) {
				bananasPerSequenceSum[el.first] += el.second;
			}
		}

		uint64_t maxBananas{};
		for (auto& el : bananasPerSequenceSum) {
			maxBananas = std::max(maxBananas, el.second);
		}

		return maxBananas;
	}

	uint64_t sumSecretNums(const std::vector<uint64_t>& nums, uint64_t itNum) const
	{
		uint64_t sum{};
		for (auto& num : nums) {
			sum += getSecretNumAfter(num, itNum);
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

	std::unordered_map<Sequence, uint64_t> possibleBananas(uint64_t secretNum, uint64_t itNum) const
	{
		if (itNum <= 4) {
			return {};
		}

		auto&& updateSeqAndNum = [this](Sequence& seq, uint64_t& secretNum) {
			uint64_t newSecretNum = getNewSecretNum(secretNum);
			int64_t priceDiff = getPrice(newSecretNum) - getPrice(secretNum);
			seq.pushNum(static_cast<uint8_t>(priceDiff));
			secretNum = newSecretNum;
		};

		std::unordered_map<Sequence, uint64_t> sequences;
		Sequence currSeq{};
		for (int i = 0; i < 4; i++) {
			updateSeqAndNum(currSeq, secretNum);
		}

		for (uint64_t i = 4; i < itNum; i++) {
			if (!sequences.contains(currSeq)) {
				sequences[currSeq] = getPrice(secretNum);
			}
			updateSeqAndNum(currSeq, secretNum);
		}

		return sequences;
	}

	uint64_t getPrice(uint64_t secretNum) const
	{
		return secretNum % 10;
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
			std::cout 
				<< "Sum of secret nums after " << numOfIterations << ": "
				<< solution.sumSecretNums(nums, numOfIterations) 
				<< std::endl
				<< "Max number of bananas: "
				<< solution.maxBananasNum(nums, numOfIterations) 
				<< std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}