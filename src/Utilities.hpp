#pragma once

#include "iostream"


#define assert(cond) \
do { \
	if (!bool(cond)) { \
		std::cout <<       \
				"[E] Assertion failed: " #cond "\n" \
                "\tFunction: " << __func__ << "\n" \
				"\tLocation: " __FILE__ ":" << std::to_string(__LINE__) << "\n"; \
	} \
} while(false);

namespace String {
	static const std::string& empty() {
		static const std::string* empty = new std::string {};
		return *empty;
	}
}
