#include "Math.hpp"

#include <cmath>


bool isEqualFloat(float a, float b, float epsilon)
{
	if (a == b) {
		return true;
	}

	const float absDiff = std::fabs(a - b);
	return absDiff < epsilon;
}
