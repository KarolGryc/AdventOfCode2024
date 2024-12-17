#include <utils.hpp>
#include <regex>
#include <queue>
#include <algorithm>

// horrible code

struct Robot
{
	aoc::Position pos;
	aoc::Vec2D velocity;

	void move(int64_t dt)
	{
		pos += velocity * dt;
	}
};

class RobotParser
{
public:
	std::vector<Robot> parseRobots(const std::vector<std::string>& lines)
	{
		std::vector<Robot> robots;
		for (const std::string& line : lines) {
			Robot r = parseRobot(line);
			robots.push_back(r);
		}
		return robots;
	}

	Robot parseRobot(const std::string& line)
	{
		aoc::Position pos = parsePosition(line);
		aoc::Vec2D vec = parseVec2D(line);
		return {pos, vec};
	}

private:
	aoc::Position parsePosition(const std::string& line)
	{
		std::regex posRegex(R"(p=(-?\d+),(-?\d+))");
		std::smatch match;
		if (std::regex_search(line, match, posRegex) && match.size() == 3) {
			return {std::stoll(match[1].str()), std::stoll(match[2].str())};
		}
		return {};
	}

	aoc::Vec2D parseVec2D(const std::string& line)
	{
		std::regex velRegex(R"(v=(-?\d+),(-?\d+))");
		std::smatch match;
		if (std::regex_search(line, match, velRegex) && match.size() == 3) {
			return {std::stoll(match[1].str()), std::stoll(match[2].str())};
		}
		return {};
	}
};

class Restroom
{
public:
	Restroom(int64_t sizeX, int64_t sizeY, const std::vector<Robot>& robots)
		: m_robots{robots}, m_sizeX{sizeX}, m_sizeY{sizeY}
	{}

	void moveAllRobots(int64_t dt)
	{
		for (Robot& robot : m_robots) {
			moveRobot(robot, dt);
		}
	}

	int64_t getSizeX() const
	{
		return m_sizeX;
	}

	int64_t getSizeY() const
	{
		return m_sizeY;
	}

	const std::vector<Robot>& getRobots() const
	{
		return m_robots;
	}

	std::vector<Robot> getRobotsQuadrant(int64_t quadrant) const
	{
		std::vector<Robot> robots;

		for(const auto& robot : m_robots) {
			if (isInQuadrant(robot.pos, quadrant)) {
				robots.push_back(robot);
			}
		}

		return robots;
	}

	void display() const 
	{
		for (int64_t y = 0; y < m_sizeY; ++y) {
			for (int64_t x = 0; x < m_sizeX; ++x) {
				std::cout << ((countRobotsOn({ x,y }) != 0) ? "R " : "  ");
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	double robotsStdPosY() const
	{
		double avg = robotsAverageY();
		double sum = 0.0;
		for (const Robot& robot : m_robots)
		{
			double diff = avg - robot.pos.y;
			sum += diff * diff;
		}
		return sqrt(sum / m_robots.size() - 1);
	}

	double robotsStdPosX() const
	{
		double avg = robotsAverageX();
		double sum = 0.0;
		for (const Robot& robot : m_robots)
		{
			double diff = avg - robot.pos.x;
			sum += diff * diff;
		}
		return sqrt(sum / m_robots.size() - 1);
	}

private:
	double robotsAverageX() const
	{
		double sum = 0.0;
		for (const auto& robot : m_robots) {
			sum += robot.pos.x;
		}
		return sum / m_robots.size();
	}

	double robotsAverageY() const
	{
		double sum = 0.0;
		for (const auto& robot : m_robots) {
			sum += robot.pos.y;
		}
		return sum / m_robots.size();
	}

	bool isInQuadrant(const aoc::Position& p, int64_t quadrant) const
	{
		if (quadrant == 0) {
			return p.x >= 0 && p.x < m_sizeX / 2 && p.y >= 0 && p.y < m_sizeY / 2;
		}
		else if (quadrant == 1) {
			return p.x > m_sizeX / 2 && p.x < m_sizeX && p.y >= 0 && p.y < m_sizeY / 2;
		}
		else if (quadrant == 2) {
			return p.x >= 0 && p.x < m_sizeX / 2 && p.y > m_sizeY / 2 && p.y < m_sizeY;
		}
		else if (quadrant == 3) {
			return p.x > m_sizeX / 2 && p.x < m_sizeX && p.y > m_sizeY / 2 && p.y < m_sizeY;
		}
		return false;
	}

	void moveRobot(Robot& robot, int64_t dt)
	{
		robot.move(dt);
		while (robot.pos.x < 0) {
			robot.pos.x += m_sizeX;
		}
		robot.pos.x %= m_sizeX;

		while (robot.pos.y < 0) {
			robot.pos.y += m_sizeY;
		}
		robot.pos.y %= m_sizeY;
	}

	int64_t countRobotsOn(const aoc::Position& p) const 
	{
		int64_t cnt = 0;
		for (auto& robot : m_robots) {
			if (robot.pos == p) {
				++cnt;
			}
		}
		return cnt;
	}

	int64_t m_sizeX;
	int64_t m_sizeY;
	std::vector<Robot> m_robots;
};


class RobotsSolution
{
public:
	uint64_t getSafetyFactor(Restroom restroom, int64_t dt)
	{
		restroom.moveAllRobots(100);
		int64_t safetyFactor = 1;
		for (int64_t i = 0; i < 4; ++i) {
			const auto robots = restroom.getRobotsQuadrant(i);
			safetyFactor *= robots.size();
		}

		return safetyFactor;
	}

	std::vector<std::pair<int64_t, double>> dtWithLeastStdProduct(
		Restroom startState, 
		int64_t maxDt) const
	{
		std::vector<std::pair<int64_t, double>> iterationsWithStd;
		for (int64_t i = 1; i <= maxDt; ++i) {
			startState.moveAllRobots(1);
			double stdX = startState.robotsStdPosX();
			double stdY = startState.robotsStdPosY();
			iterationsWithStd.emplace_back(i, stdX * stdY);
		}

		std::sort(iterationsWithStd.begin(), iterationsWithStd.end(),
			[](const auto& a, const auto& b) { return a.second < b.second; });

		return iterationsWithStd;
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

	RobotParser parser;
	RobotsSolution solution;
	for (const std::string& arg : runArgs) {
		try {
			std::vector<std::string> lines{ aoc::loadFile(arg) };
			auto robots = parser.parseRobots(lines);
			Restroom restroom (101, 103, robots);
			uint64_t dt = 100;
			std::cout << "Safety factor after " << dt << " s: "
				<< solution.getSafetyFactor(restroom, 100)
				<< std::endl;

			auto sortedResults = solution.dtWithLeastStdProduct(restroom, 10000);
			std::cout << "Element with christmas tree (smallest std): "
				<< sortedResults.front().first
				<< std::endl;

			Restroom room(restroom);
			room.moveAllRobots(sortedResults.front().first);
			room.display();
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return 0;
}
