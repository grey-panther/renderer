#pragma once

#include "forwards.hpp"
#include <cmath>


struct Vec3
{
public:
	float x, y, z;

public:
	explicit Vec3(float x = 0, float y = 0, float z = 0)
			: x(x)
			, y(y)
			, z(z)
	{}

	Vec3& operator*=(float multiplier);

	Vec3& operator/=(float divider);

	Vec3& operator+=(float addition);

	Vec3& operator+=(const Vec3& other);

	Vec3& operator-=(const Vec3& other);

	Vec3& normalize();

	float length() const;

	Vec3i round() const;

	static float distance(const Vec3& p0, const Vec3& p1);

	/**
	 * @brief Векторное произведение.
	 */
	static Vec3 crossMultiply(const Vec3& a, const Vec3& b);

	/**
	 * @brief Скалярное произведение.
	 */
	static float dotMultiply(const Vec3& a, const Vec3& b);
};

Vec3 operator+(const Vec3& v1, const Vec3& v2);

Vec3 operator-(const Vec3& v1, const Vec3& v2);

Vec3 operator*(float multiplier, Vec3 v);

inline Vec3 operator*(const Vec3& v, float multiplier) { return operator*(multiplier, v); }

Vec3 operator/(const Vec3& v, float divider);
