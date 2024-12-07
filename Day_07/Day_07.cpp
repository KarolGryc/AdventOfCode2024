#include <iostream>
#include <vector>
#include <sstream>
#include <functional>
#include "utils.hpp"

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

		int firstElem = m_elements.front();
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
		
		if (sum > m_result) {
			return false;
		}

		int64_t newSum = myOp(sum, *currElemenet);
		auto nextElemIt = currElemenet + 1;
		for (const Operation& op : callOps) {
			if (validityCheck(newSum, nextElemIt, op, callOps)) {
				return true;
			}
		}
		return false;
	}


	ResultT m_result;
	std::vector<ElemT> m_elements;
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
		const Equation::Operations& operations
	) const
	{
		uint64_t sum{};
		for (const Equation& equation : equations) {
			if (equation.isValid(operations)) {
				sum += equation.getResult();
			}
		}
		return sum;
	}

	Equation::Operation m_mul = [](Equation::ResultT a, Equation::ElemT b) {
		return a * b;
	};

	Equation::Operation m_add = [](Equation::ResultT a, Equation::ElemT b) {
		return a + b;
	};

	// May be more efficient using log10
	Equation::Operation m_con = [](Equation::ResultT a, Equation::ElemT b) {
		return std::stoull(std::to_string(a) + std::to_string(b));
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

			std::cout << "For file " << arg << std::endl
				<< "Sum of valid equations using (+, *): " 
				<< solution.sumValidEquationsAddMul(equations) << std::endl
				<< "Sum of valid equations using (+, *, ||): " 
				<< solution.sumValidEquationsAddMulCon(equations) << std::endl;
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}
		
	}
	return 0;
}