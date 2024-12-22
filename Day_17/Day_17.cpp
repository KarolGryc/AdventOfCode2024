#include <utils.hpp>
#include <regex>
#include <unordered_map>
#include <functional>
#include <queue>
#include <deque>

using Program = std::vector<uint8_t>;

class Computer
{
public:
	Computer(std::function<void(uint8_t)> output, uint64_t regA = 0, uint64_t regB = 0, uint64_t regC = 0)
		: m_output{output}, m_regA{regA}, m_regB{regB}, m_regC{regC}
	{}

	void setRegA(uint64_t regA)
	{
		m_regA = regA;
	}

	void executeProgram(const Program& program)
	{
		while (m_instrPtr < (int64_t)program.size()) {
			auto [opCode, operand] = fetch(program);
			Instruction instr = decode(opCode);
			execute(instr, operand);
			incrementInstrPtr();
		}
	}

	void setOutput(std::function<void(uint8_t)> output)
	{
		m_output = output;
	}

private:
	void incrementInstrPtr()
	{
		m_instrPtr += 2;
	}

	std::pair<uint8_t, uint8_t> fetch(const Program& program) const 
	{
		return { program[m_instrPtr], program[m_instrPtr + 1]};
	}

	using Instruction = void(Computer::*)(uint8_t);
	Instruction decode(uint8_t opCode)
	{
		auto instr = m_instructions[opCode];
		return instr;
	}

	void execute(Instruction instr, uint8_t operand)
	{
		(this->*(instr))(operand);
	}

	void adv(uint8_t operand)
	{
		m_regA /= (1ull << comboOperandVal(operand));
	} 

	void bxl(uint8_t operand)
	{
		m_regB ^= operand;
	}

	void bst(uint8_t operand)
	{
		m_regB = comboOperandVal(operand) & 7;
	}

	void jnz(uint8_t operand)
	{
		if (m_regA != 0) {
			m_instrPtr = static_cast<int64_t>(operand) - 2;
		}
	}

	void bxc(uint8_t operand)
	{
		m_regB ^= m_regC;
	}

	void out(uint8_t operand)
	{
		m_output(static_cast<uint8_t>(comboOperandVal(operand) & 7));
	}

	void bdv(uint8_t operand)
	{
		m_regB = m_regA / (1ull << comboOperandVal(operand));
	}

	void cdv(uint8_t operand)
	{
		m_regC = m_regA / (1ull << comboOperandVal(operand));
	}

	const uint64_t comboOperandVal(uint8_t comboOperand) const
	{
		if (comboOperand <= 3) {
			return comboOperand;
		}
		else if (comboOperand == 4) {
			return m_regA;
		}
		else if (comboOperand == 5) {
			return m_regB;
		}
		else if (comboOperand == 6) {
			return m_regC;
		}
		else {
			throw std::invalid_argument("Invalid combo operand.");
		}
	}


	static constexpr Instruction m_instructions[] {
			&adv,
			&bxl,
			&bst,
			&jnz,
			&bxc,
			&out,
			&bdv,
			&cdv,
	};

	std::function<void(uint8_t)> m_output;
	int64_t m_instrPtr{};
	uint64_t m_regA;
	uint64_t m_regB;
	uint64_t m_regC;
};

class ChronospatialComputerParser
{
public:
	Computer parseComputer(
		const std::vector<std::string>& lines, 
		std::function<void(uint8_t)> outputCallback) const
	{
		std::unordered_map<char, uint64_t> regs = parseRegisters(lines);
		return { outputCallback, regs['A'], regs['B'], regs['C'] };
	}

	Program parseProgram(const std::vector<std::string>& lines)
	{
		for (const auto& line : lines) {
			Program p = parseProgram(line);
			if (!p.empty() && p.size() % 2 == 0) {
				return p;
			}
		}

		return {};
	}

	Program parseProgram(const std::string& line)
	{
		std::regex regex(R"(Program:\s((\d+,)*\d+))");

		Program program{};
		std::smatch match;
		if (std::regex_match(line, match, regex)) {
			std::string data = match[1].str();
			std::regex dataRegex(R"(\d+)");
			auto it = std::sregex_iterator(data.begin(), data.end(), dataRegex);
			auto end = std::sregex_iterator();

			while (it != end) {
				program.push_back(std::stoi(it->str()));
				++it;
			}
		}
		return program;
	}

private:
	std::unordered_map<char, uint64_t> parseRegisters(const std::vector<std::string>& lines) const
	{
		std::regex regex(R"(Register\s([A-C]):\s(\d+))");
		std::unordered_map<char, uint64_t> registers;

		for (const auto& line : lines) {
			std::smatch match;
			if (std::regex_search(line, match, regex)) {
				char regName = match[1].str()[0];
				uint64_t val = std::stoull(match[2]);
				registers[regName] = val;
			}
		}

		return registers;
	}
};

class StinkySolution
{
public:
	uint64_t firstSelfPrintingValRegA(const Program& program) const
	{
		uint64_t result = UINT64_MAX;
		std::queue<uint64_t> checkedNums{ {0ull} };
		Program programReversed{ program.rbegin(), program.rend() };

		while (!checkedNums.empty()) {
			uint64_t num = checkedNums.front();
			checkedNums.pop();
			for (int i = 0; i <= 7; i++) {
				// prepare computer and container for output
				std::deque<uint8_t> printedNums;
				Computer computer([&](uint8_t x) { printedNums.push_front(x); });

				// set value of regA and execute
				uint64_t regA = (num << 3) + i;
				computer.setRegA(regA);
				computer.executeProgram(program);

				// if computer printed partially correct result finish or continue
				// else ignore path
				if (resultsOverlap(programReversed, printedNums)) {
					if (programReversed.size() == printedNums.size()) {
						result = std::min(regA, result);
					}
					else {
						checkedNums.push(regA);
					}
				}
			}
		}
		
		return result;
	}

private:
	bool resultsOverlap(const std::vector<uint8_t>& a, const std::deque<uint8_t>& b) const
	{
		if (b.size() > a.size()) {
			return false;
		}
		
		size_t i = 0;
		for (auto it = b.begin(); it != b.end(); ++it) {
			if (*it != a[i++]) {
				return false;
			}
		}

		return true;
	}
};

int main(int argc, char* args[])
{
	auto runArgs = aoc::argsToString(argc - 1, args + 1);

	if (runArgs.empty()) {
		const std::string defaultFile = "input.txt";
		std::cerr << "No args given, running default file: " 
			<< defaultFile 
			<< std::endl;
		runArgs.push_back(defaultFile);
	}

	ChronospatialComputerParser parser;
	StinkySolution solution;
	for (const std::string& arg : runArgs) {
		try {
			auto lines = aoc::loadFile(arg);
			Computer computer = parser.parseComputer(lines,
				[printed = false](uint8_t response) mutable {
					std::cout << (printed ? "," : "") << (int)response;
					printed = true;
				}
			);
			Program program = parser.parseProgram(lines);
			computer.executeProgram(program);

			std::cout << std::endl
				<< "Minimal self printing register A value: "
				<< solution.firstSelfPrintingValRegA(program)
				<< std::endl;

		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}