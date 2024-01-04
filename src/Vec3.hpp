#pragma once

#include "forwards.hpp"

#include <cmath>
#include <iosfwd>


struct Vec3
{
public:
	Vec3() = default;

	Vec3(float x, float y, float z)
			: x(x)
			, y(y)
			, z(z)
	{}

	Vec3(int x, int y, int z)
			: Vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z))
	{}

	explicit Vec3(float scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
	{}

	explicit Vec3(const Vec3i& intVec);

	Vec3& operator*=(float multiplier);

	Vec3& operator/=(float divider);

	Vec3& operator+=(float addition);

	Vec3& operator+=(const Vec3& other);

	Vec3& operator-=(const Vec3& other);

	Vec3& normalize();

	[[nodiscard]]
	Vec3 normalized() const;

	[[nodiscard]]
	float length() const;

	[[nodiscard]]
	Vec3i rounded() const;

	static float distance(const Vec3& p0, const Vec3& p1);

	/**
	 * @brief Векторное произведение.
	 */
	static Vec3 crossMultiply(const Vec3& a, const Vec3& b);

	/**
	 * @brief Скалярное произведение.
	 */
	static float dotMultiply(const Vec3& a, const Vec3& b);

public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};

Vec3 operator+(const Vec3& v1, const Vec3& v2);

Vec3 operator-(const Vec3& v1, const Vec3& v2);

Vec3 operator*(float multiplier, Vec3 v);

inline Vec3 operator*(const Vec3& v, float multiplier) { return operator*(multiplier, v); }

Vec3 operator/(const Vec3& v, float divider);

std::ostream& operator<<(std::ostream& os, const Vec3& v);
