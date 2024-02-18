#include "Vec2i.hpp"

#include "Vec2.hpp"
#include <cmath>


Vec2i::Vec2i(const Vec2& floatVec)
		: x(static_cast<int>(std::round(floatVec.x)))
		, y(static_cast<int>(std::round(floatVec.y)))
{
}


Vec2i operator+(const Vec2i& v1, const Vec2i& v2)
{
	const int x = v1.x + v2.x;
	const int y = v1.y + v2.y;
	return {x, y};
}


Vec2i operator-(const Vec2i& v1, const Vec2i& v2)
{
	const int x = v1.x - v2.x;
	const int y = v1.y - v2.y;
	return {x, y};
}
