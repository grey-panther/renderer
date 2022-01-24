#pragma once

#define assert(cond) \
do { \
	if (!bool(cond)) { \
		std::cout <<       \
				"[E] Assertion failed: " #cond "\n" \
                "\tFunction: " << __func__ << "\n" \
				"\tLocation: " __FILE__ ":" << std::to_string(__LINE__) << "\n"; \
	} \
} while(false);
