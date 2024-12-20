#include <utils.hpp>
#include <regex>
#include <unordered_map>

using Program = std::vector<uint8_t>;

class ChronospatialComputer
{
public:
	ChronospatialComputer(std::ostream& output, uint64_t regA = 0, uint64_t regB = 0, uint64_t regC = 0)
		: m_output{output}, m_regA{regA}, m_regB{regB}, m_regC{regC}
	{}

	void execute(const Program& program)
	{
		m_alreadyPrinted = false;
		
		while (m_instrPtr < (int64_t)program.size()) {
			uint8_t opCode = program[m_instrPtr];
			uint8_t operand = program[m_instrPtr + 1];
			Instruction instr = fetchInstruction(opCode);
			(this->*(instr))(operand);
			incrementInstrPtr();
		}
	}

private:
	void incrementInstrPtr()
	{
		m_instrPtr += 2;
	}

	using Instruction = void(ChronospatialComputer::*)(uint8_t);
	Instruction fetchInstruction(uint8_t opCode)
	{
		auto instr = m_instructions[opCode];
		return instr;
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
		if (m_alreadyPrinted) {
			m_output << ',';
		}
		m_output << (comboOperandVal(operand) & 7);
		m_alreadyPrinted = true;
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

	bool m_alreadyPrinted = false;
	std::ostream& m_output;
	int64_t m_instrPtr{};
	uint64_t m_regA;
	uint64_t m_regB;
	uint64_t m_regC;
};

class ChronospatialComputerParser
{
public:
	ChronospatialComputer parseComputer(const std::vector<std::string>& lines)
	{
		std::unordered_map<char, uint64_t> regs = parseRegisters(lines);
		return { std::cout, regs['A'], regs['B'], regs['C'] };
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
	for (const std::string& arg : runArgs) {
		try {
			auto lines = aoc::loadFile(arg);
			ChronospatialComputer computer = parser.parseComputer(lines);
			Program program = parser.parseProgram(lines);
			computer.execute(program);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}