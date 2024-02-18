#pragma once

#include "forwards.hpp"


struct Vec2i
{
	Vec2i() = default;

	Vec2i(int x, int y)
			: x(x)
			, y(y)
	{}

	explicit Vec2i(int scalar)
			: x(scalar)
			, y(scalar)
	{}

	explicit Vec2i(const Vec2& floatVec);

public:
	int x = 0;
	int y = 0;
};

Vec2i operator+(const Vec2i& v1, const Vec2i& v2);

Vec2i operator-(const Vec2i& v1, const Vec2i& v2);
