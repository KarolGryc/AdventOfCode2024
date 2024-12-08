#include "utils.hpp"
#include <unordered_map>
#include <unordered_set>

struct Vec2D
{
	int x;
	int y;
};

struct Position
{
	int x;
	int y;
	
	bool operator==(const Position& p) const { return p.x == x && p.y == y; }
	Vec2D operator-(const Position& p) const { return { x - p.x, y - p.y }; }
	Position operator+(const Vec2D& v) const { return { x + v.x, y + v.y }; }
	Position operator+=(const Vec2D& v) { return *this = *this + v; }
};

template<>
struct std::hash<Position>
{
	std::size_t operator() (const Position& p) const 
	{
		return std::hash<int>()(p.x) ^ hash<int>()(p.y) << 1;
	}
};


class AntennaMap
{
public:
	using FreqName = char;
	using DataMap = std::unordered_map<FreqName, std::vector<Position>>;
	AntennaMap(size_t sizeX, size_t sizeY, const DataMap& antennaPositions) 
		: m_sizeX{ sizeX }, 
		m_sizeY{ sizeY },
		m_antennaPositions{ antennaPositions }
	{}

	AntennaMap(size_t sizeX, size_t sizeY, DataMap&& antennaPositions) 
		: m_sizeX{ sizeX }, 
		m_sizeY{ sizeY },
		m_antennaPositions{ std::move(antennaPositions) }
	{}

	void display()
	{
		for (auto& [key, val] : m_antennaPositions) {
			std::cout << key << std::endl;
			for (auto& el : val) {
				std::cout << "<" << el.x << "," << el.y << "> ";
			}
			std::cout << std::endl;
		}
	}

	std::unordered_set<Position> getAntinodes() const
	{
		std::unordered_set<Position> result;

		for (const auto& [freq, pos] : m_antennaPositions) {
			const auto& antinodes = getAntinodes(freq);
			for (auto& antinode : antinodes) {
				result.insert(antinode);
			}
		}

		return result;
	}

	std::unordered_set<Position> getAntinodes(FreqName freqName) const
	{
		if (!m_antennaPositions.contains(freqName)) {
			return {};
		}

		const auto& freqPositions = m_antennaPositions.at(freqName);
		std::unordered_set<Position> antinodePositions;

		// we suppose that antinode can be on another antenna
		int size = freqPositions.size();
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (i == j) continue;

				Position aPos = freqPositions[i];
				Position bPos = freqPositions[j];
				Vec2D offset = bPos - aPos;
				Position antinodePos = bPos + offset;

				if (contains(antinodePos)) {
					antinodePositions.insert(antinodePos);
				}
			}
		}

		return antinodePositions;
	}

private:
	bool contains(const Position& p) const 
	{
		return p.y >= 0 && p.y < m_sizeY && p.x >= 0 && p.x < m_sizeX;
	}

	size_t m_sizeX;
	size_t m_sizeY;
	DataMap m_antennaPositions;
};


class ResonantCollinearitySolution
{
public:
	size_t countAntinodesLocation(const AntennaMap& map) const 
	{
		return map.getAntinodes().size();
	}
};


class AntennaParser
{
public:
	AntennaMap parseMap(const std::vector<std::string>& mapLines)
	{
		if (mapLines.empty()) {
			throw std::runtime_error("Map is empty!");
		}

		AntennaMap::DataMap map;
		size_t sizeY = mapLines.size();
		size_t sizeX = mapLines[0].size();
		for (int y = 0; y < sizeY; ++y) {
			if (sizeX != mapLines[y].size()) {
				throw std::runtime_error("Map is not rectangular!");
			}

			for (int x = 0; x < sizeX; ++x) {
				char c = mapLines[y][x];
				if (c != emptyChar) {
					map[c].push_back({ x,y });
				}
			}
		}

		return AntennaMap{ sizeX, sizeY, map };
	}

private:
	static constexpr char emptyChar = '.';
};



int main(int argc, char* args[])
{
	auto runArgs{ aoc::argsToString(argc - 1, args + 1) };

	if (runArgs.empty()) {
		std::cerr << "No arguments given!";
		return 1;
	}

	AntennaParser parser;
	for (const std::string& arg : runArgs) {
		try {
			auto dataLines = aoc::loadFile(arg);
			AntennaMap map = parser.parseMap(dataLines);
			
			ResonantCollinearitySolution solution;
			std::cout << "Number of unique locations for file "
				<< arg << ": "
				<< solution.countAntinodesLocation(map)
				<< std::endl;
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}

	}
	return 0;
}
