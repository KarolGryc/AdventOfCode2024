#include <vector>
#include <string>
#include <unordered_set>
#include <chrono>
#include <iostream>
#include <array>

namespace aoc {
	std::vector<std::string> argsToString(int argc, char* args[]);
	std::vector<std::string> loadFile(const std::string& fileName);
	uint8_t countDigits(uint64_t num);

	template<typename T>
	bool setsIntersect(const std::unordered_set<T>& s1, const std::unordered_set<T>& s2)
	{
		for (auto& el : s1) {
			if (s2.contains(el)) {
				return true;
			}
		}
		return false;
	}

	template<typename T>
	void insertSet(const std::unordered_set<T>& from, std::unordered_set<T>& to)
	{
		for (auto& el : from) {
			to.insert(el);
		}
	}

	struct Vec2D
	{
		int64_t x;
		int64_t y;
		constexpr Vec2D operator+(const Vec2D& v) const {
			return {x + v.x, y + v.y};
		}

		constexpr Vec2D operator*(int64_t scalar) const {
			return {x * scalar, y * scalar};
		}

		constexpr bool operator==(const Vec2D& v) const {
			return v.x == x && v.y == y;
		}

		static constexpr std::array<Vec2D, 4> getUnitVectors() {
			return { {{1, 0},{0, 1},{-1, 0},{0, -1}} };
		}
	};

	struct Position
	{
		int64_t x;
		int64_t y;

		constexpr bool operator==(const Position& p) const { return p.x == x && p.y == y; }
		constexpr Vec2D operator-(const Position& p) const { return { x - p.x, y - p.y }; }
		constexpr Position operator+(const Vec2D& v) const { return { x + v.x, y + v.y }; }
		constexpr Position operator+=(const Vec2D& v) { return *this = *this + v; }
	};

	class SimpleClock {
	private:
		std::chrono::high_resolution_clock::time_point startTime;
		std::chrono::high_resolution_clock::time_point endTime;
		bool isRunning = false;

	public:
		void start() {
			if(isRunning) {
				return;
			}
			startTime = std::chrono::high_resolution_clock::now();
			isRunning = true;
		}

		void end() {
			endTime = std::chrono::high_resolution_clock::now();
			isRunning = false;

			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			std::cout << "Execution time: " << duration << " ms" << std::endl;
		}
	};
}


template<>
struct std::hash<aoc::Position>
{
	std::size_t operator() (const aoc::Position& p) const 
	{
		return std::hash<int64_t>()(p.x) ^ hash<int64_t>()(p.y) << 1;
	}
};
