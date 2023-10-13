#pragma once


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

public:
	int x = 0;
	int y = 0;
};
