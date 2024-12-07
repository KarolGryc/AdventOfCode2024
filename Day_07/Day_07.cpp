#include <iostream>
#include <vector>
#include <sstream>
#include <functional>
#include <numeric>
#include <execution>
#include <cmath>
#include "utils.hpp"


#define CUT_UNCORRECT_BRANCHES 1
#define PARALLEL_EXECUTION 1  // parallel is slower in debug!!!
#define STRING_CONCAT 0
#if STRING_CONCAT == 0
	#define LOG_CONCAT 0
	#if LOG_CONCAT == 0
		#include "unreadableOptimisations.hpp"
	#endif
#endif

class Equation
{
public:
	using ResultT = uint64_t;
	using ElemT = uint32_t;
	using Operation = std::function<ResultT(ResultT, ElemT)>;
	using Operations = std::vector<Operation>;

	Equation(ResultT result, const std::vector<ElemT>& elements)
		: m_result{ result }, m_elements{ elements }
	{}


	Equation(ResultT result, std::vector<ElemT>&& elements)
		: m_result{ result }, m_elements{ std::move(elements) }
	{}


	ResultT getResult() const
	{
		return m_result;
	}
	

	bool isValid(const Operations& operations) const
	{
		if (m_elements.empty()) {
			return m_result == 0;
		}

		ElemT firstElem = m_elements.front();
		ElemIterator nextElemIt = m_elements.cbegin() + 1;

		for (const Operation& op : operations) {
			if (validityCheck(firstElem, nextElemIt, op, operations)) {
				return true;
			}
		}
		return false;
	}


private:
	using ElemIterator = std::vector<ElemT>::const_iterator;

	bool validityCheck(
		ResultT sum, 
		ElemIterator currElemenet, 
		Operation myOp, 
		const Operations& callOps
	) const
	{
		if (currElemenet == m_elements.cend()) {
			return sum == m_result;
		}

		ResultT newSum = myOp(sum, *currElemenet);

	#if CUT_UNCORRECT_BRANCHES == 1
		if (newSum > m_result) {
			return false;
		}
	#endif

		auto nextElemIt = currElemenet + 1;
		for (const Operation& op : callOps) {
			if (validityCheck(newSum, nextElemIt, op, callOps)) {
				return true;
			}
		}
		return false;
	}


	const ResultT m_result;
	const std::vector<ElemT> m_elements;
	const ElemIterator m_end;
};
using Equations = std::vector<Equation>;



class EquationParser
{
public:
	Equations parseEquations(const std::vector<std::string>& equations) const
	{
		std::vector<Equation> result;
		for (const auto& equation : equations) {
			Equation parsedEquation = parseEquation(equation);
			result.emplace_back(parsedEquation);
		}

		return result;
	}


	Equation parseEquation(const std::string& equation) const
	{
		std::stringstream ss(equation);
		Equation::ResultT result;
		char separator;
		ss >> result >> separator;
		
		
		std::vector<Equation::ElemT> elements;
		Equation::ElemT element;
		while (ss >> element) {
			elements.push_back(element);
		}

		if (separator != ':' || (ss.fail() && !ss.eof())) {
			std::string message = "Equation" + equation + " is ill formed!";
			throw std::invalid_argument(message);
		}

		return Equation(result, std::move(elements));
	}
};


// Fields should be static
class BridgeRepairSolution
{
public:
	uint64_t sumValidEquationsAddMul(const Equations& equations) const
	{
		return sumValidEquations(equations, { m_add, m_mul });
	}


	uint64_t sumValidEquationsAddMulCon(const Equations& equations) const
	{
		return sumValidEquations(equations, { m_add, m_mul, m_con });
	}


private:
	uint64_t sumValidEquations(
		const Equations& equations,
		const Equation::Operations& ops
	) const
	{
#if PARALLEL_EXECUTION == 0
		uint64_t sum{};
		for (const Equation& equation : equations) {
			if (equation.isValid(ops)) {
				sum += equation.getResult();
			}
		}
		return sum;
#else
		return std::transform_reduce(std::execution::par_unseq, 
			equations.begin(), equations.end(), 
			0ull,
			std::plus<uint64_t>(),
			[&ops](const Equation& eq) -> uint64_t {
				if (eq.isValid(ops)) {
					return eq.getResult();
				}
				return 0ull;
			});
#endif
	}

	Equation::Operation m_mul = [](Equation::ResultT a, Equation::ElemT b) {
		return a * b;
	};

	Equation::Operation m_add = [](Equation::ResultT a, Equation::ElemT b) {
		return a + b;
	};

	// May be more efficient using log10
	Equation::Operation m_con = [](Equation::ResultT a, Equation::ElemT b) {
#if STRING_CONCAT == 1
		return std::stoull(std::to_string(a) + std::to_string(b));
#else
	#if LOG_CONCAT == 1
		int numberOfDigits = static_cast<int>(log10(b)) + 1;
		return a * static_cast<uint64_t>(pow(10, numberOfDigits)) + b;
	#else		
		int numberOfDigits = unreadable::numDigits(b);
		return a * unreadable::powersOf10[numberOfDigits] + b;
	#endif
#endif
	};
};


int main(int argc, char* args[])
{
	auto runArgs{ aoc::argsToString(argc - 1, args + 1) };

	if (runArgs.empty()) {
		std::cerr << "No arguments given!";
		return 1;
	}

	EquationParser eqParser;
	BridgeRepairSolution solution;
	for (const std::string& arg : runArgs) {
		try {
			std::vector<std::string> lines = aoc::loadFile(arg);
			std::vector<Equation> equations = eqParser.parseEquations(lines);

			
			aoc::SimpleClock c;
			c.start();
			std::cout << "For file " << arg << std::endl
				<< "Sum of valid equations using (+, *): "
				<< solution.sumValidEquationsAddMul(equations) << std::endl;
			c.end();

			c.start();
			std::cout << "Sum of valid equations using (+, *, ||): " 
				<< solution.sumValidEquationsAddMulCon(equations) << std::endl;
			c.end();
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}
		
	}
	return 0;
}