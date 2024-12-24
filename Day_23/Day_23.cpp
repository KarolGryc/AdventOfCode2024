#include <utils.hpp>
#include <regex>
#include <set>
#include <unordered_map>
#include <queue>

using ConnectionMap = std::unordered_map<std::string, std::unordered_set<std::string>>;

class ComputerParser
{
public:
	ConnectionMap parseNetwork(const std::vector<std::string>& lines) const
	{
		ConnectionMap map;
		for (auto& line : lines) {
			auto [first, second] = parseComputerPair(line);
			map[first].insert(second);
			map[second].insert(first);
		}

		return map;
	}

private:
	std::pair<std::string, std::string> parseComputerPair(const std::string& line) const
	{
		std::regex regex(R"(^([a-z]+)\-([a-z]+)$)");
		std::smatch match;
		if (std::regex_match(line, match, regex)) {
			return{ match[1].str(), match[2].str() };
		}
		
		throw std::invalid_argument("Invalid computer connection string.");
	}
};

struct Clique
{
	std::set<std::string> elements;
	bool operator==(const Clique& c) const 
	{
		if (elements.size() != c.elements.size()) {
			return false;
		}

		for (auto& n : elements) {
			if (!c.elements.contains(n)) {
				return false;
			}
		}

		return true;
	}

	bool containsNodeStartingWith(char c) const
	{
		for (auto& el : elements) {
			if (el[0] == c) {
				return true;
			}
		}
		return false;
	}
};

template<>
struct std::hash<Clique>
{
	size_t operator()(const Clique& c) const 
	{
		size_t hash{};
		for (auto& el : c.elements) {
			hash = (hash << 1) ^ std::hash<std::string>{}(el);
		}
		return hash;
	}
};

class SolutionLAN
{
public:
	uint64_t countCliques(const ConnectionMap& map) const
	{
		std::unordered_set<Clique> cliques;
		for (const auto& [nodeName, _] : map) {
			const auto& foundCliques = getCliques(map, {}, nodeName, 3);
			aoc::insertSet(foundCliques, cliques);
		}

		for (auto it = cliques.begin(); it != cliques.end();) {
			if (it->elements.size() != 3) {
				it = cliques.erase(it);
			}
			else if (!it->containsNodeStartingWith('t')) {
				it = cliques.erase(it);
			}
			else {
				++it;
			}
		}

		return cliques.size();
	}

private:
	std::unordered_set<Clique> getCliques(
		const ConnectionMap& connections,
		std::set<std::string> previousNodes,
		const std::string& nodeName,
		int64_t maxSize) const
	{
		if (!connections.contains(nodeName)) {
			return {};
		}

		if (maxSize < 0) {
			return {};
		}

		if (!nodeHasConnections(connections, nodeName, previousNodes)) {
			Clique clique;
			clique.elements = previousNodes;
			return { clique };
		}

		previousNodes.insert(nodeName);
		std::unordered_set<Clique> cliques;
		auto& neighbours = connections.at(nodeName);
		for (auto& neighbour : neighbours) {
			const auto& foundCliques = getCliques(connections, previousNodes, neighbour, maxSize - 1);
			aoc::insertSet(foundCliques, cliques);
		}

		return cliques;
	}

	bool nodeHasConnections(
		const ConnectionMap& connections,
		const std::string& nodeName,
		const std::set<std::string>& toNodes) const 
	{
		if (!connections.contains(nodeName)) {
			return false;
		}

		const auto& nodeConnections = connections.at(nodeName);
		for (auto& node : toNodes) {
			if (!nodeConnections.contains(node)) {
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
	
	ComputerParser parser;
	SolutionLAN solution;
	for (const std::string& arg : runArgs) {
		try {
			static constexpr uint64_t numOfIterations = 2000;
			auto lines = aoc::loadFile(arg);
			auto network = parser.parseNetwork(lines);
			std::cout << solution.countCliques(network);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}