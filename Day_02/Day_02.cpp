#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// utilities
std::vector<std::string> argsToString(int argc, char* args[]);

template<typename T>
bool isInRange(T num, T min, T max);

// should be a class
using Report = std::vector<int>;
Report parseReport(const std::string& report);
std::vector<Report> parseFileOfReports(const std::string& fileName);
bool isSafeIncrease(int current, int next, int minInterval, int maxInterval);
bool isSafeDecrease(int current, int next, int minInterval, int maxInterval);
bool isReportSafe(const Report& report, int minInterval, int maxInterval);
bool isReportSafeDamp(const Report& report, int minInterval, int maxInterval);
uint64_t countSafeReports(const std::vector<Report>& reports);
uint64_t countSafeReportsDamp(const std::vector<Report>& reports);




int main(int argc, char* args[])
{
	std::vector<std::string> runtimeArgs{ argsToString(argc - 1, args + 1) };

	if (runtimeArgs.empty()) {
		std::cout << "No runtime arguments given!";
		return 1;
	}

	for (const std::string& argument : runtimeArgs) {
		try {
			std::vector<Report> reports{ parseFileOfReports(argument) };
			std::cout << "Number of safe reports in file "
				<< argument << ": "
				<< countSafeReports(reports) << std::endl;
			std::cout << "Number of safe reports (with dampener) in file "
				<< argument << ": "
				<< countSafeReportsDamp(reports) << std::endl;
		}
		catch (std::runtime_error& e) {
			std::cout << e.what();
		}
	}

	return 0;
}



std::vector<std::string> argsToString(int argc, char* args[])
{
	std::vector<std::string> parsedArgs(argc);
	for (size_t i = 0; i < argc; i++) {
		parsedArgs[i] = args[i];
	}
	return parsedArgs;
}



Report parseReport(const std::string& report)
{
	Report result;

	Report::value_type num;
	std::istringstream ss(report);
	while (ss >> num) {
		result.push_back(num);
	}

	if (ss.fail() && !ss.eof()) {
		throw std::invalid_argument("Uncorrect data in report!");
	}

	return result;
}



std::vector<Report> parseFileOfReports(const std::string& fileName)
{
	std::vector<Report> reports;
	std::ifstream file(fileName);
	if (!file) {
		throw std::runtime_error("Cannot open file: " + fileName);
	}

	std::string line;
	while (std::getline(file, line)) {
		try {
			reports.emplace_back(parseReport(line));
		}
		catch (std::invalid_argument& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	return reports;
}

bool isSafeIncrease(int current, int next, int minInterval, int maxInterval)
{
	int diff = next - current;
	return isInRange(diff, minInterval, maxInterval);
}

bool isSafeDecrease(int current, int next, int minInterval, int maxInterval)
{
	int diff = current - next;
	return isInRange(diff, minInterval, maxInterval);
}

bool isReportSafe(const Report& report, int minInterval, int maxInterval)
{
	size_t n = report.size();
	if (n < 2) {
		return true;
	}

	bool isIncreasing = report[0] < report[1];
	auto isSafe = isIncreasing ? isSafeIncrease : isSafeDecrease;

	for (size_t i = 0; i < n - 1; ++i) {
		if (!isSafe(report[i], report[i + 1], minInterval, maxInterval))
			return false;
	}

	return true;
}


uint64_t countSafeReports(const std::vector<Report>& reports)
{
	constexpr int minDiff = 1;
	constexpr int maxDiff = 3;
	return std::count_if(reports.begin(), reports.end(),
		[&](const auto& r) { return isReportSafe(r, minDiff, maxDiff); });
}

uint64_t countSafeReportsDamp(const std::vector<Report>& reports)
{
	constexpr int minDiff = 1;
	constexpr int maxDiff = 3;
	return std::count_if(reports.begin(), reports.end(),
		[&](const auto& r) { return isReportSafeDamp(r, minDiff, maxDiff); });
}

bool isReportSafeDamp(const Report& report, int minInterval, int maxInterval)
{
	size_t n = report.size();
	if (n <= 2) {
		return true;
	}

	bool isIncreasing = report[0] < report[1];
	auto isSafe = isIncreasing ? isSafeIncrease : isSafeDecrease;

	for (size_t i = 0; i < n - 1; i++) {
		if (!isSafe(report[i], report[i + 1], minInterval, maxInterval)) {
			Report withoutCurr{ report };
			Report withoutNext{ report };
			withoutCurr.erase(withoutCurr.begin() + i);
			withoutNext.erase(withoutNext.begin() + i + 1);
			bool ret =	isReportSafe(withoutCurr, minInterval, maxInterval) ||
						isReportSafe(withoutNext, minInterval, maxInterval);
			if (i > 0) {
				Report withoutPrev{ report };
				withoutPrev.erase(withoutPrev.begin() + i - 1);
				ret = ret || isReportSafe(withoutPrev, minInterval, maxInterval);
			}
			return ret;
		}
	}

	return true;
}



template<typename T>
bool isInRange(T num, T min, T max) {
	return num >= min && num <= max;
}