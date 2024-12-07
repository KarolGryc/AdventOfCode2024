#include "unreadableOptimisations.hpp"


// from Stack, works well
int unreadable::numDigits(uint64_t x) {
	if (x >= 10000000000) {
		if (x >= 100000000000000) {
			if (x >= 10000000000000000) {
				if (x >= 100000000000000000) {
					if (x >= 1000000000000000000)
						return 19;
					return 18;
				}
				return 17;
			}
			if (x >= 1000000000000000)
				return 16;
			return 15;
		}
		if (x >= 1000000000000) {
			if (x >= 10000000000000)
				return 14;
			return 13;
		}
		if (x >= 100000000000)
			return 12;
		return 11;
	}
	if (x >= 100000) {
		if (x >= 10000000) {
			if (x >= 100000000) {
				if (x >= 1000000000)
					return 10;
				return 9;
			}
			return 8;
		}
		if (x >= 1000000)
			return 7;
		return 6;
	}
	if (x >= 100) {
		if (x >= 1000) {
			if (x >= 10000)
				return 5;
			return 4;
		}
		return 3;
	}
	if (x >= 10)
		return 2;
	return 1;
}