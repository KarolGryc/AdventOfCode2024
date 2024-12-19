#include <utils.hpp>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <queue>

enum class Dir
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	ERROR
};

std::array<Dir,4> getAllDirs()
{
	using enum Dir;
	return {NORTH, WEST, SOUTH, EAST};
}

aoc::Vec2D getDirVector(Dir dir) {
	switch (dir) {
		using enum Dir;
		case NORTH: return { 0,-1 };
		case SOUTH: return { 0,1 };
		case WEST:	return { -1,0 };
		case EAST:	return { 1,0 };
		case ERROR: return { 0,0 };
	}
	throw std::invalid_argument("Invalid direction");
}

constexpr Dir getDirFromVector(const aoc::Vec2D& vec) {
	using enum Dir;
	if (vec == aoc::Vec2D{ 1,0 })
		return EAST;
	else if (vec == aoc::Vec2D{ -1,0 })
		return WEST;
	else if (vec == aoc::Vec2D{ 0,1 })
		return SOUTH;
	else if (vec == aoc::Vec2D{ 0,-1 })
		return EAST;
	else
		return ERROR;
}

Dir getOppositeDir(Dir dir)
{
	switch (dir) {
		using enum Dir;
		case NORTH: return SOUTH;
		case EAST:	return WEST;
		case SOUTH: return NORTH;
		case WEST:	return EAST;
	}

	throw std::invalid_argument("Unknown argument!");
}


struct Node
{
	constexpr Node(aoc::Position position, Dir direction)
		: pos{position}, dir{direction}
	{}

	aoc::Position pos;
	Dir dir;

	bool operator==(const Node& n) const {
		return n.pos == pos && n.dir == dir;
	}
};

template<>
struct std::hash<Node> {
	std::size_t operator()(const Node& n) const {
		return std::hash<aoc::Position>{}(n.pos) ^
			std::hash<int>{}(static_cast<int>(n.dir)) << 1;
	}
};

class Maze
{
public:
	using Graph = std::unordered_map<Node, std::unordered_map<Node, uint64_t>>;

	Maze(const Graph& g)
		: m_graph(g)
	{}

	Maze(Graph&& g)
		: m_graph(std::move(g))
	{}

	const Graph& getGraph() const
	{
		return m_graph;
	}

private:
	Graph m_graph;
};



class MazeParser
{
public:
	auto parseMaze(const std::vector<std::string>& lines) const
		-> Maze
	{
		if (!isCorrectFormat(lines)) {
			throw std::invalid_argument("Invalid labirynth format!");
		}

		Maze::Graph graph;
		for (int y = 1; y < lines.size() - 1; ++y) {
			for (int x = 1; x < lines[y].size() - 1; ++x) {
				aoc::Position pos{ x,y };
				if (isNode(lines, pos)) {
					auto posNodes = generateNodesWithTurnEdges(pos);
					graph.insert(posNodes.begin(), posNodes.end());
				}
			}
		}

		for (auto& [node, edges] : graph) {
			auto edge = generateNeigbourEdge(lines, node);
			if (edge.first != errorNode) {
				edges.insert(edge);
			}
		}

		return { std::move(graph) };
	}

	aoc::Position findStart(const std::vector<std::string>& lines) const
	{
		return findFirst(startField, lines);
	}

	aoc::Position findEnd(const std::vector<std::string>& lines) const
	{
		return findFirst(endField, lines);
	}

private:
	auto generateNeigbourEdge(const std::vector<std::string>& map, const Node& n) const
		-> std::pair<Node, uint64_t>
	{
		aoc::Vec2D dirVec = getDirVector(n.dir);
		
		uint64_t cost = 1;
		aoc::Position nextPos = n.pos + dirVec;
		while (canVisit(map, nextPos)) {
			if (isNode(map, nextPos)) {
				return { {nextPos, n.dir}, cost };
			}
			cost++;
			nextPos += dirVec;
		}

		return { errorNode, UINT64_MAX };
	}

	auto generateNodesWithTurnEdges(const aoc::Position& pos) const
		-> std::unordered_map<Node, std::unordered_map<Node, uint64_t>>
	{
		std::unordered_map<Node, std::unordered_map<Node, uint64_t>> result;
		for (const Node& node : generateNodes(pos)) {
			auto nodeEdges = generateTurnEdges(node);
			result[node].insert(nodeEdges.begin(), nodeEdges.end());
		}

		return result;
	}

	std::unordered_map<Node, uint64_t> generateTurnEdges(const Node& n) const
	{
		std::unordered_map<Node, uint64_t> edges;
		std::array<Dir, 4> dirs = getAllDirs();
		for (Dir dir : dirs) {
			if (dir != n.dir && dir != getOppositeDir(n.dir)) {
				edges.insert({ Node{n.pos,dir}, 1000 });
			}
		}

		return edges;
	}

	std::unordered_set<Node> generateNodes(const aoc::Position& p) const
	{
		std::unordered_set<Node> nodes;
		std::array<Dir, 4> dirs = getAllDirs();
		for (Dir dir : dirs) {
			nodes.insert(Node{ p, dir });
		}
		return nodes;
	}

	bool isCorrectFormat(const std::vector<std::string>& map) const
	{
		static constexpr size_t MIN_SIZE_Y = 4;
		static constexpr size_t MIN_SIZE_X = 4;

		if (map.size() < MIN_SIZE_Y) {
			return false;
		}

		for (const auto& line : map) {
			if (line.size() < MIN_SIZE_X) {
				return false;
			}
		}

		if (map.front().find_first_not_of(wallField) != std::string::npos ||
			map.back().find_first_not_of(wallField) != std::string::npos) {
			return false;
		}

		for (const auto& line : map) {
			if (line.front() != wallField || line.back() != wallField) {
				return false;
			}
		}

		return true;
	}

	aoc::Position findFirst(char searchedChar, const std::vector<std::string>& lines) const
	{
		for (int y = 0; y < (int)lines.size(); ++y) {
			for (int x = 0; x < (int)lines[y].size(); ++x) {
				if (lines[y][x] == searchedChar) {
					return { x,y };
				}
			}
		}
		return errorPos;
	}

	bool isNode(const std::vector<std::string>& map, const aoc::Position& p) const
	{
		bool visitable = canVisit(map, p);
		uint8_t availablePaths = countAvailablePaths(map, p);
		bool straightPath = isStraightPath(map, p);
		return canVisit(map, p) && 
			(countAvailablePaths(map, p) != 2 || !isStraightPath(map, p));
	}

	bool isStraightPath(const std::vector<std::string>& map, const aoc::Position& p) const
	{
		return canVisit(map[p.y][p.x + 1]) == canVisit(map[p.y][p.x - 1]) &&
			canVisit(map[p.y - 1][p.x]) == canVisit(map[p.y + 1][p.x]);
	}

	uint8_t countAvailablePaths(const std::vector<std::string>& map, const aoc::Position& p) const
	{
		auto dirs = getAllDirs();
		uint8_t pathsCount{};
		for (Dir dir : dirs) {
			aoc::Position checkedPos = p + getDirVector(dir);
			if (mapContains(map, checkedPos) && canVisit(map, checkedPos)) {
				++pathsCount;
			}
		}

		return pathsCount;
	}

	bool mapContains(const std::vector<std::string>& map, const aoc::Position& p) const
	{
		return p.y >= 0 && p.y < (int)map.size() && p.x >= 0 && p.x < (int)map.at(p.y).size();
	}

	bool canVisit(const std::vector<std::string>& map, const aoc::Position& p) const
	{
		return canVisit(map[p.y][p.x]);
	}

	bool canVisit(char c) const
	{
		return c == normalField || c == startField || c == endField;
	}

	static constexpr char normalField = '.';
	static constexpr char startField = 'S';
	static constexpr char endField = 'E';
	static constexpr char wallField = '#';
	static constexpr aoc::Position errorPos = { -1,-1 };
	static constexpr Node errorNode = { errorPos, Dir::ERROR };

};


class MazeSolution
{
public:
	uint64_t findMinScore(
		const Maze& maze, 
		const aoc::Position& startPos,
		Dir startDir,
		const aoc::Position& endPos) const
	{
		Maze::Graph graph = maze.getGraph();

		Node startNode{ startPos, startDir };
		if (!graph.contains(startNode)) {
			throw std::invalid_argument("Graph doesn't contain start node");
		}

		std::unordered_set<Node> visitedNodes;
		std::unordered_map<Node, uint64_t> distances;

		auto cmp = [&distances](const Node& a, const Node& b) {
			return distances[a] > distances[b];
		};
		std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);

		distances[startNode] = 0;
		for (auto& [node, edges] : graph) {
			if (node != startNode) {
				distances[node] = UINT64_MAX;
			}
		}
		pq.push(startNode);

		while (!pq.empty()) {
			Node currNode = pq.top();
			pq.pop();
			const auto& neighbours = graph[currNode];
			for (const auto& [node, dist] : neighbours) {
				if (!visitedNodes.contains(node)) {
					uint64_t newDist = distances[currNode] + dist;
					if (newDist < distances[node]) {
						distances[node] = newDist;
						pq.push(node);
					}
				}
			}
			visitedNodes.insert(currNode);
		}
		
		uint64_t minDistToEnd = UINT64_MAX;
		for (const auto& dir : getAllDirs()) {
			Node endNode{ endPos, dir };
			minDistToEnd = std::min(distances[endNode], minDistToEnd);
		}

		return minDistToEnd;
	}
};

int main(int argc, char* args[])
{
	auto runArgs = aoc::argsToString(argc - 1, args + 1);

	if (runArgs.empty()) {
		const std::string defaultFile = "tinyExample.txt";
		std::cerr << "No args given, running default file: " 
			<< defaultFile 
			<< std::endl;
		runArgs.push_back(defaultFile);
	}

	MazeParser parser;
	MazeSolution solution;
	for (const std::string& arg : runArgs) {
		try {
			auto lines = aoc::loadFile(arg);
			Maze maze = parser.parseMaze(lines);
			aoc::Position startPos = parser.findStart(lines);
			aoc::Position endPos = parser.findEnd(lines);
			std::cout << "Result: " 
				<< solution.findMinScore(maze, startPos, Dir::EAST, endPos)
				<< std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}