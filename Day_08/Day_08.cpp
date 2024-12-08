#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <execution>
#include "utils.hpp"

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

	void displayVisualizationWithNodes() const 
	{
		std::vector<std::string> lines(m_sizeY);
		std::for_each(std::execution::par, lines.begin(), lines.end(),
			[this](std::string& line) { line = std::string(m_sizeX, '.');});

		for (const auto& [freqName, positions] : m_antennaPositions) {
			for (const auto& pos : positions) {
				lines[pos.y][pos.x] = freqName;
			}
		}

		const auto& nodes = getAntinodes();
		for (const auto& node : nodes) {
			lines[node.y][node.x] = '#';
		}

		for (const auto& line : lines) {
			std::cout << line << std::endl;
		}
	}

	std::unordered_set<Position> getAntinodes() const
	{
		std::unordered_set<Position> result;
		for (const auto& [freq, pos] : m_antennaPositions) {
			const auto& antinodes = getAntinodes(freq);
			result.insert(antinodes.begin(), antinodes.end());
		}

		return result;
	}

	std::unordered_set<Position> getAntinodesInLine() const
	{
		std::unordered_set<Position> result;
		for (const auto& [freq, pos] : m_antennaPositions) {
			const auto& antinodes = getAntinodesInLine(freq);
			result.insert(antinodes.begin(), antinodes.end());
		}

		return result;
	}

	std::unordered_set<Position> getAntinodes(FreqName freqName) const
	{
		if (!m_antennaPositions.contains(freqName)) {
			return {};
		}

		std::unordered_set<Position> antinodePositions;
		const auto& freqPositions = m_antennaPositions.at(freqName);

		for (const Position& aPos : freqPositions) {
			for (const Position& bPos : freqPositions) {
				if (&aPos == &bPos) {
					continue;
				}

				Vec2D offset = bPos - aPos;
				Position antinodePos = bPos + offset;
				if (contains(antinodePos)) {
					antinodePositions.insert(antinodePos);
				}
			}
		}

		return antinodePositions;
	}

	std::unordered_set<Position> getAntinodesInLine(FreqName freqName) const
	{
		if (!m_antennaPositions.contains(freqName)) {
			return {};
		}

		std::unordered_set<Position> antinodePositions;
		const auto& freqPositions = m_antennaPositions.at(freqName);

		for (const Position& aPos : freqPositions) {
			for (const Position& bPos : freqPositions) {
				if (&aPos == &bPos) {
					continue;
				}

				Vec2D offset = bPos - aPos;
				Position antinodePos = bPos;
				while (contains(antinodePos)) {
					antinodePositions.insert(antinodePos);
					antinodePos += offset;
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

	size_t countAntinodesInLines(const AntennaMap& map) const
	{
		return map.getAntinodesInLine().size();
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
			std::cout << "File " << arg << ": " << std::endl
				<< "\tUnique antinodes: " 
				<< solution.countAntinodesLocation(map)
				<< std::endl
				<< "\tUnique antinodes in lines: "
				<< solution.countAntinodesInLines(map)
				<< std::endl;
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}

	}
	return 0;
}
