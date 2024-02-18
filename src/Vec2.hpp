#pragma once

#include "forwards.hpp"
#include <iosfwd>


struct Vec2
{
	Vec2() = default;

	Vec2(float x, float y)
			: x(x)
			, y(y)
	{}

	explicit Vec2(float scalar)
			: x(scalar)
			, y(scalar)
	{}

	explicit Vec2(const Vec2i& intVec);

	Vec2& operator*=(float multiplier);

	Vec2& operator/=(float divider);

	Vec2& operator+=(float addition);

	Vec2& operator+=(const Vec2& other);

	Vec2& operator-=(const Vec2& other);

	Vec2& normalize();

	[[nodiscard]]
	Vec2 normalized() const;

	[[nodiscard]]
	float length() const;

	[[nodiscard]]
	Vec2i rounded() const;

public:
	float x = 0.f;
	float y = 0.f;
};

[[nodiscard]]
Vec2 operator+(const Vec2& v1, const Vec2& v2);

[[nodiscard]]
Vec2 operator-(const Vec2& v1, const Vec2& v2);

[[nodiscard]]
Vec2 operator*(float multiplier, const Vec2& v);

[[nodiscard]]
inline Vec2 operator*(const Vec2& v, float multiplier) { return operator*(multiplier, v); }

[[nodiscard]]
Vec2 operator/(const Vec2& v, float divider);

std::ostream& operator<<(std::ostream& os, const Vec2& v);
