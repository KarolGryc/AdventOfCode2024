#include <utils.hpp>
#include <unordered_map>
#include <queue>
#include <iomanip>

class RaceTrack
{
public:
	enum class Field
	{
		TRACK,
		WALL
	};

	RaceTrack(
		aoc::Position startPos, aoc::Position endPos,
		size_t sizeX, size_t sizeY, 
		Field defaultField = Field::TRACK)
		: m_sizeX{sizeX}, m_sizeY{sizeY},
		m_startPos{startPos}, m_endPos{endPos},
		m_fields{sizeX * sizeY, defaultField}
	{}

	size_t sizeX() const
	{
		return m_sizeX;
	}
	
	size_t sizeY() const
	{
		return m_sizeY;
	}

	const aoc::Position& getStartPos() const
	{
		return m_startPos;
	}

	const aoc::Position& getEndPos() const
	{
		return m_endPos;
	}

	void setField(const aoc::Position& p, Field type)
	{
		if (contains(p)) {
			at(p) = type;
		}
	}

	Field getType(const aoc::Position& p) const
	{
		return at(p);
	}

	bool contains(const aoc::Position& p) const
	{
		return p.x >= 0 && p.x < (int64_t)sizeX() && 
			p.y >= 0 && p.y < (int64_t)sizeY();
	}

private:
	Field& at(const aoc::Position& p)
	{
		return m_fields[p.y * m_sizeY + p.x];
	}

	const Field& at(const aoc::Position& p) const
	{
		return m_fields[p.y * m_sizeY + p.x];
	}

	aoc::Position m_startPos;
	aoc::Position m_endPos;
	size_t m_sizeX;
	size_t m_sizeY;
	std::vector<Field> m_fields;
};


class RaceTrackParser
{
public:
	RaceTrack parseRaceTrack(const std::vector<std::string>& lines)
	{
		aoc::Position startPos = findStartPos(lines);
		if (startPos == errorPos) {
			throw std::invalid_argument("Argument doesn't contain start point");
		}

		aoc::Position endPos = findEndPos(lines);
		if (endPos == errorPos) {
			throw std::invalid_argument("Argument doesn't contain end point");
		}

		auto [sizeX, sizeY] = getDimensions(lines);

		RaceTrack track(startPos, endPos, sizeX, sizeY, RaceTrack::Field::TRACK);
		for (int64_t y = 0; y < (int64_t)lines.size(); ++y) {
			for (int64_t x = 0; x < (int64_t)lines[y].size(); ++x) {
				if (lines[y][x] == wallSymbol) {
					track.setField({ x,y }, RaceTrack::Field::WALL);
				}
			}
		}

		return track;
	}

private:
	bool isRectangular(const std::vector<std::string>& lines) const
	{
		if (lines.empty()) {
			return false;
		}

		size_t sizeX = lines.front().size();
		for (const auto& line : lines) {
			if (line.size() != sizeX) {
				return false;
			}
		}

		return true;
	}

	std::pair<size_t, size_t> getDimensions(const std::vector<std::string>& lines) const
	{
		if (lines.empty()) {
			throw std::invalid_argument("Given board is empty.");
		}

		if (!isRectangular(lines)) {
			throw std::invalid_argument("Board is not rectangular.");
		}

		size_t sizeX = lines.front().size();
		size_t sizeY = lines.size();

		return { sizeX, sizeY };
	}

	aoc::Position findStartPos(const std::vector<std::string>& lines)
	{
		return findSymbol(lines, startSymbol);
	}

	aoc::Position findEndPos(const std::vector<std::string>& lines)
	{
		return findSymbol(lines, endSymbol);
	}

	aoc::Position findSymbol(const std::vector<std::string>& lines, char symbol)
	{
		for (int64_t y = 0; y < (int64_t)lines.size(); ++y) {
			const std::string& line = lines[y];
			for (int64_t x = 0; x < (int64_t)line.size(); ++x) {
				if (line[x] == symbol) {
					return { x,y };
				}
			}
		}
		return errorPos;
	}

	static constexpr char startSymbol = 'S';
	static constexpr char endSymbol = 'E';
	static constexpr char wallSymbol = '#';
	static constexpr char trackSymbol = '.';
	static constexpr aoc::Position errorPos = { -1,-1 };
};


class ProgramPathFinder
{
public:
	std::unordered_map<aoc::Position, uint64_t> getDistances(const RaceTrack& track) const
	{
		aoc::Position startPos = track.getStartPos();
		uint64_t startCost{};
		std::queue<aoc::Position> toVisit{ { startPos } };
		std::unordered_map<aoc::Position, uint64_t> visited{ {startPos, startCost} };

		while (!toVisit.empty()) {
			aoc::Position currPos = toVisit.front();
			if (currPos == track.getEndPos()) {
				break;
			}
			toVisit.pop();
			uint64_t currCost = visited[currPos];
			auto neighbourTracks = neighbours(currPos, track);
			for (auto& field : neighbourTracks) {
				if (!visited.contains(field)) {
					toVisit.push(field);
					visited.insert({ field, currCost + 1 });
				}
			}
		}

		return visited;
	}

	uint64_t numOfSkips(const RaceTrack& track, int64_t cheatTime, int64_t minSaved) const
	{
		if (cheatTime <= 1 || minSaved <= 0) {
			return 0;
		}

		std::unordered_map<aoc::Position, uint64_t> fields = getDistances(track);
		auto offsets = generateOffsetVecs(cheatTime);
		uint64_t cnt{};
		for (auto& field : fields) {
			const auto& [currPos, _] = field;
			for (const auto& offset : offsets) {
				const auto& nPos = currPos + offset;
				if (fields.contains(nPos) && isSkipValid(field, { nPos, fields[nPos] }, minSaved)) {
					++cnt;
				}
			}
		}

		return cnt;
	}

private:
	bool isSkipValid(
		const std::pair<aoc::Position, uint64_t>& oldField,
		const std::pair<aoc::Position, uint64_t>& newField,
		int64_t minSavedCost
	) const
	{
		int64_t oldCost = static_cast<int64_t>(oldField.second);
		int64_t newCost = static_cast<int64_t>(newField.second);
		aoc::Position oldPos = oldField.first;
		aoc::Position newPos = newField.first;
		aoc::Vec2D distanceV = newPos - oldPos;
		int64_t distance = std::abs(distanceV.x) + std::abs(distanceV.y);
		int64_t savedCost = newCost - oldCost - distance;
		return savedCost >= minSavedCost;
	}

	std::vector<aoc::Vec2D> generateOffsetVecs(int64_t radius) const
	{
		if (radius <= 0) {
			return {};
		}

		std::vector<aoc::Vec2D> offsets;
		for (int64_t yOff = -radius; yOff <= radius; ++yOff) {
			for (int64_t xOff = -radius; xOff <= radius; ++xOff) {
				if (!(yOff == 0 && xOff == 0) &&std::abs(yOff) + std::abs(xOff) <= radius) {
					offsets.push_back({ xOff,yOff });
				}
			}
		}

		return offsets;
	}

	std::vector<aoc::Position> neighbours(
		const aoc::Position& pos,
		const RaceTrack& track,
		RaceTrack::Field fieldType = RaceTrack::Field::TRACK) const
	{
		std::vector<aoc::Position> result;
		for (auto& v : aoc::Vec2D::getUnitVectors()) {
			aoc::Position newPos = pos + v;
			if (track.contains(newPos) && track.getType(newPos) == fieldType) {
				result.push_back(newPos);
			}
		}

		return result;
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
	
	RaceTrackParser parser;
	ProgramPathFinder solution;
	for (const std::string& arg : runArgs) {
		try {
			auto lines = aoc::loadFile(arg);
			auto track = parser.parseRaceTrack(lines);
			static constexpr int savedCost = 100;
			static constexpr int cheatTime = 20;
			std::cout << "Num of skips saving " << savedCost 
				<< " for " << cheatTime << " ps: "
				<< solution.numOfSkips(track, cheatTime, savedCost) << std::endl;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}