#include <list>
#include <cmath>
#include <utils.hpp>

struct Segment
{
	using ID = uint32_t;
	enum {
		EMPTY,
		FILLED
	} type;
	ID id;
	uint32_t size;

	bool isFilled() const {
		return type == FILLED;
	}

	bool isEmpty() const {
		return type == EMPTY;
	}

	Segment cut(uint32_t cuttedSize) {
		uint32_t newObjectSize = std::min(cuttedSize, size);
		this->size -= newObjectSize;
		return { type, id, newObjectSize };
	};
};

class Memory
{
public:
	using Iterator = std::list<Segment>::iterator;
	using ReverseIterator = std::list<Segment>::reverse_iterator;
	using ConstIterator = std::list<Segment>::const_iterator;

	void pushDataSegment(uint32_t size) {
		m_entries.emplace_back(Segment::FILLED, getNewSegmentId(), size);
	}

	void pushEmptySegment(uint32_t size) {
		m_entries.emplace_back(Segment::EMPTY, 0, size);
	}

	void insert(const Iterator& it, Segment val) {
		m_entries.insert(it, val);
	}

	void insert(const ReverseIterator& it, Segment val) {
		m_entries.insert(it.base(), val);
	}

	Iterator begin() {
		return m_entries.begin();
	}

	Iterator end() {
		return m_entries.end();
	}

	ReverseIterator rbegin() {
		return m_entries.rbegin();
	}

	ReverseIterator rend() {
		return m_entries.rend();
	}

	ConstIterator cbegin() const {
		return m_entries.cbegin();
	}

	ConstIterator cend() const {
		return m_entries.cend();
	}

	void splitSegment(const Iterator& it, uint32_t cutOffSize) {
		Segment remainder = it->cut(cutOffSize);
		insert(std::next(it), remainder);
	}

	void splitSegment(const ReverseIterator& it, uint32_t cutOffSize) {
		Segment remainder = it->cut(cutOffSize);
		insert(std::next(it), remainder);
	}

	uint64_t controlSum() {
		uint64_t sum{};
		uint64_t counter{};
		for (const auto& el : m_entries) {
			for (uint32_t i = 0; i < el.size; i++) {
				sum += el.id * counter++;
			}
		}
		return sum;
	}

private:
	Segment::ID getNewSegmentId() {
		return m_maxSegmentId++;
	}

	Segment::ID m_maxSegmentId{};
	std::list<Segment> m_entries;
};


class MemoryOrganizer
{
public:
	virtual void organize(Memory& memory) const = 0;

protected:
	bool areOnTheSameSegment(
		const Memory::Iterator& it,
		const Memory::ReverseIterator& rit
	) const
	{
		return it == std::prev(rit.base());
	}
};


class SquishyFragmenter : public MemoryOrganizer
{
public:
	void organize(Memory& memory) const override {
		Memory::Iterator emptyIt = memory.begin();
		Memory::ReverseIterator filledIt = memory.rbegin();

		while (true) {
			// find next empty space
			while (emptyIt->isFilled()) {
				if (areOnTheSameSegment(emptyIt, filledIt)) {
					return;
				}
				++emptyIt;
			}

			// find next filled space
			while (filledIt->isEmpty()) {
				if (areOnTheSameSegment(emptyIt, filledIt)) {
					return;
				}
				++filledIt;
			}
			
			// split file if needed
			uint64_t remainderSize = std::abs(int64_t(emptyIt->size) - filledIt->size);
			if (emptyIt->size > filledIt->size) {
				memory.splitSegment(emptyIt, static_cast<uint32_t>(remainderSize));
			}
			else if(emptyIt->size < filledIt->size) {
				memory.splitSegment(filledIt, static_cast<uint32_t>(remainderSize));
			}

			std::swap(*emptyIt, *filledIt);
		}
	}
};


class SpaciousDefragmenter : public MemoryOrganizer
{
public:
	// could probably be heavily optimised
	void organize(Memory& memory) const override {
		Memory::ReverseIterator filledIt = memory.rbegin();
		const Memory::ReverseIterator rend = memory.rend();
		while (filledIt != rend) {
			// find next filled space
			while (filledIt != rend && filledIt->isEmpty()) {
				++filledIt;
			}

			if (filledIt == rend) {
				return;
			}
			
			// check all possible blank spaces
			Memory::Iterator emptyIt = memory.begin();
			while (!areOnTheSameSegment(emptyIt, filledIt)) {
				if (emptyIt->isEmpty()) {
					if (emptyIt->size > filledIt->size) {
						int64_t remainderSize = (int64_t)emptyIt->size - filledIt->size;
						memory.splitSegment(emptyIt, remainderSize);
						std::swap(*emptyIt, *filledIt);
						break;
					}
					else if (emptyIt->size == filledIt->size) {
						std::swap(*emptyIt, *filledIt);
						break;
					}
				}
				++emptyIt;
			}

			++filledIt;
		}
	}
};

class MemoryParser
{
public:
	Memory parseMemory(const std::string& line)
	{
		Memory memory;
		bool isEmptySpace = false;
		for (auto c : line) {
			uint32_t size = atoi(&c);
			if (size != 0) { // branch prediction inefficient :(
				if (isEmptySpace) {
					memory.pushEmptySegment(size);
				}
				else {
					memory.pushDataSegment(size);
				}
			}
			isEmptySpace = !isEmptySpace;
		}
		return memory;
	}
};


int main(int argc, char* args[])
{
	std::vector<std::string> runArgs{ aoc::argsToString(argc - 1, args + 1) };

	if (runArgs.empty()) {
		std::cerr << "No files to run given!";
		return 1;
	}

	MemoryParser parser;
	SquishyFragmenter fragmenter;
	SpaciousDefragmenter defragmenter;
	for (const auto& arg : runArgs) {
		for (const auto& line : aoc::loadFile(arg)) {
			
			Memory memory{ parser.parseMemory(line) };
			Memory squished = memory;
			Memory defragmented = memory;
			fragmenter.organize(squished);
			defragmenter.organize(defragmented);
			std::cout << "For file " << arg << ": " << std::endl
				<< "Control sum after squishy fragmentation: "
				<< squished.controlSum()
				<< "Control sum after defragmentation: "
				<< defragmented.controlSum();
		}
	}

	return 0;
}
