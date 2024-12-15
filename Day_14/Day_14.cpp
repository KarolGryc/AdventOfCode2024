#include <utils.hpp>
#include <regex>

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

	std::vector<Robot> getRobotsQuadrant(uint8_t quadrant) const
	{
		std::vector<Robot> robots;

		for(const auto& robot : m_robots) {
			if (isInQuadrant(robot.pos, quadrant)) {
				robots.push_back(robot);
			}
		}

		return robots;
	}
private:
	bool isInQuadrant(const aoc::Position& p, uint8_t quadrant) const
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
		auto& x = robot.pos.x;
		auto& y = robot.pos.y;
		auto& vx = robot.velocity.x;
		auto& vy = robot.velocity.y;
//		std::cout << "Old pos: " << x << " " << y << "V=" << vx << " " <<  vy <<  std::endl;
		robot.move(dt);
		while (robot.pos.x < 0) {
			robot.pos.x += m_sizeX;
		}
		robot.pos.x %= m_sizeX;

		while (robot.pos.y < 0) {
			robot.pos.y += m_sizeY;
		}
		robot.pos.y %= m_sizeY;
//		std::cout << "New pos: " << x << " " << y << "V=" << vx << " " <<  vy <<  std::endl;
	}

	int64_t m_sizeX;
	int64_t m_sizeY;
	std::vector<Robot> m_robots;
};


class RobotsSolution
{
public:
	uint64_t getSafetyFactor(const Restroom& restroom)
	{
		int64_t safetyFactor = 1;
		for (int64_t i = 0; i < 4; ++i) {
			const auto robots = restroom.getRobotsQuadrant(i);
			std::cout << std::endl << robots.size() <<std::endl;
			safetyFactor *= robots.size();
		}

		return safetyFactor;
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
		std::vector<std::string> lines{ aoc::loadFile(arg) };
		auto robots = parser.parseRobots(lines);
		Restroom restroom (101, 103, robots);
		restroom.moveAllRobots(100);
		std::cout << "Safety factor: "
			<< solution.getSafetyFactor(restroom)
			<< std::endl;
	}

	return 0;
}
