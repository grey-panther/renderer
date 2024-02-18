#include "Vec2.hpp"

#include "Math.hpp"
#include "Vec2i.hpp"

#include <cmath>
#include <ostream>
#include <string>


Vec2::Vec2(const Vec2i& intVec)
		: x(static_cast<float>(intVec.x))
		, y(static_cast<float>(intVec.y))
{
}


Vec2& Vec2::operator*=(float multiplier)
{
	x *= multiplier;
	y *= multiplier;
	return *this;
}


Vec2& Vec2::operator/=(float divider)
{
	x /= divider;
	y /= divider;
	return *this;
}


Vec2& Vec2::operator+=(float addition)
{
	x += addition;
	y += addition;
	return *this;
}


Vec2& Vec2::operator+=(const Vec2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}


Vec2& Vec2::operator-=(const Vec2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}


Vec2& Vec2::normalize()
{
	const float length = this->length();
	if (length <= DEFAULT_EPSILON) {
		return *this;
	}

	const float inverseLength = 1.f / length;

	x *= inverseLength;
	y *= inverseLength;

	return *this;
}


Vec2 Vec2::normalized() const
{
	auto v = *this;
	v.normalize();
	return v;
}


float Vec2::length() const
{
	return std::sqrt(x * x + y * y);
}


Vec2i Vec2::rounded() const
{
	return Vec2i(*this);
}


Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
	const float x = v1.x + v2.x;
	const float y = v1.y + v2.y;
	return {x, y};
}


Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	const float x = v1.x - v2.x;
	const float y = v1.y - v2.y;
	return {x, y};
}


Vec2 operator*(float multiplier, const Vec2& v)
{
	const float x = multiplier * v.x;
	const float y = multiplier * v.y;
	return {x, y};
}


Vec2 operator/(const Vec2& v, float divider)
{
	const float x = v.x / divider;
	const float y = v.y / divider;
	return {x, y};
}


std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
	os << "Vec2(" << std::to_string(v.x) << ", " << std::to_string(v.y) << ")";
	return os;
}
