#include <vector>
#include <string>
#include <unordered_set>
#include <chrono>
#include <iostream>

namespace aoc{
	std::vector<std::string> argsToString(int argc, char* args[]);
	std::vector<std::string> loadFile(const std::string& fileName);

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