#pragma once

#include "Vec3.hpp"
#include "Vec2i.hpp"


struct Vec3i
{
public:
	Vec3i() = default;

	Vec3i(int x, int y, int z)
			: x(x)
			, y(y)
			, z(z)
	{}

	explicit Vec3i(int scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
	{}

	explicit Vec3i(const Vec3& point)
			: x(static_cast<int>(std::round(point.x)))
			, y(static_cast<int>(std::round(point.y)))
			, z(static_cast<int>(std::round(point.z)))
	{}

	[[nodiscard]]
	Vec2i xy() const { return Vec2i(x, y); }

	[[nodiscard]]
	Vec2i xz() const { return Vec2i(x, z); }

	[[nodiscard]]
	Vec2i yz() const { return Vec2i(y, z); }

	Vec3i& operator-=(const Vec3i& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vec3i& operator+=(const Vec3i& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vec3i& operator*=(int multiplier)
	{
		x *= multiplier;
		y *= multiplier;
		z *= multiplier;
		return *this;
	}

	Vec3i& operator*=(float multiplier)
	{
		x = static_cast<int>(std::round(x * multiplier));
		y = static_cast<int>(std::round(y * multiplier));
		z = static_cast<int>(std::round(z * multiplier));
		return *this;
	}

public:
	int x = 0;
	int y = 0;
	int z = 0;
};

Vec3i operator-(const Vec3i& v1, const Vec3i& v2);

Vec3i operator+(const Vec3i& v1, const Vec3i& v2);

Vec3i operator*(Vec3i v, int multiplier);

Vec3i operator*(int multiplier, Vec3i v);

Vec3i operator*(Vec3i v, float multiplier);

Vec3i operator*(float multiplier, Vec3i v);
