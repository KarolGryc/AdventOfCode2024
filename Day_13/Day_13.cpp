#include <utils.hpp>
#include <regex>
#include <unordered_map>

struct Button
{
	uint64_t cost;
	aoc::Vec2D offset;
};

class ClawMachine
{
public:
	ClawMachine() = default;

	ClawMachine(const Button& a, const Button& b, const aoc::Position& prizePos)
		: m_buttonA{a}, m_buttonB{b}, m_prizePos{prizePos}
	{}

	const Button& getButtonA() const {
		return m_buttonA;
	}

	const Button& getButtonB() const {
		return m_buttonB;
	}

	const aoc::Position& getPrizePos() const {
		return m_prizePos;
	}

private:
	Button m_buttonA;
	Button m_buttonB;
	aoc::Position m_prizePos;
};

class ClawMachineParser
{
public:
	std::vector<ClawMachine> parseClawMachines(const std::vector<std::string>& lines) const
	{
		std::vector<ClawMachine> result;
		for (int i = 0; i < lines.size(); i += 4) {
			const std::string lineA = lines[i];
			const std::string lineB = lines[i + 1];
			const std::string linePrize = lines[i + 2];
			
			ClawMachine machine = parseClawMachine(lineA, lineB, linePrize);
			result.emplace_back(std::move(machine));
		}

		return result;
	}

	ClawMachine parseClawMachine(
		const std::string& lineButtonA,
		const std::string& lineButtonB,
		const std::string& linePrizePos) const
	{
		aoc::Position btnPosA = parsePos(lineButtonA);
		aoc::Position btnPosB = parsePos(lineButtonB);
		aoc::Position prizePos = parsePos(linePrizePos);

		Button btnA = { 3, {btnPosA.x, btnPosA.y} };
		Button btnB = { 1, {btnPosB.x, btnPosB.y} };
		return {btnA, btnB, prizePos};
	}

private:
	aoc::Position parsePos(const std::string& line) const
	{
		std::regex xNumRegex(R"(X[\+-=]\d{1,})");
		std::regex yNumRegex(R"(Y[\+-=]\d{1,})");
		std::smatch xMatch;
		std::smatch yMatch;
		std::regex_search(line, xMatch, xNumRegex);
		std::regex_search(line, yMatch, yNumRegex);
		std::string matchX = xMatch.str().substr(1);
		std::string matchY = yMatch.str().substr(1);
		if (matchX.front() == '=') {
			matchX = matchX.substr(1);
		}
		if (matchY.front() == '=') {
			matchY = matchY.substr(1);
		}
		return {std::stoi(matchX), std::stoi(matchY)};
	}
};


class PrizeFinder
{
public:
	uint64_t getMinimumPrizeCost(const ClawMachine& machine)
	{
		// solve by linear algebra
		const Button& btnA = machine.getButtonA();
		const Button& btnB = machine.getButtonB();
		const aoc::Position& goalPos = machine.getPrizePos();

	}
};


int main(int argc, char* args[])
{
	auto runArgs = aoc::argsToString(argc - 1, args + 1);

	if (runArgs.empty()) {
		const std::string defaultFile = "example.txt";
		std::cerr << "No args given, running default file: " << defaultFile;
		runArgs.push_back(defaultFile);
	}

	ClawMachineParser parser;
	PrizeFinder finder;
	for (const std::string& arg : runArgs) {
		std::vector<std::string> lines{ aoc::loadFile(arg) };
		auto machines = parser.parseClawMachines(lines);
		uint64_t sum{};
		for (auto& machine : machines) {
			sum += finder.getMinimumPrizeCost(machine);
		}
		std::cout << sum;
	}

	return 0;
}