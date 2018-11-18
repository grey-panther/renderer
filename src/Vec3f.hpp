#ifndef RENDERER_VEC3F_HPP
#define RENDERER_VEC3F_HPP

#include "forwards.hpp"
#include <cmath>


struct Vec3f
{
public:
	float x, y, z;

public:
	explicit Vec3f(float x = 0, float y = 0, float z = 0)
			: x(x)
			, y(y)
			, z(z)
	{}

	Vec3f& operator*=(float multiplier);

	Vec3f& operator+=(float addition);

	Vec3f& operator+=(const Vec3f& other);

	Vec3f& operator-=(const Vec3f& other);

	Vec3f& normalize();

	float length() const;

	Vec3i round() const;

	static float distance(const Vec3f& p0, const Vec3f& p1);

	/**
	 * @brief Векторное произведение.
	 */
	static Vec3f crossMultiply(const Vec3f& a, const Vec3f& b);

	/**
	 * @brief Скалярное произведение.
	 */
	static float dotMultiply(const Vec3f& a, const Vec3f& b);
};

Vec3f operator+(const Vec3f& v1, const Vec3f& v2);

Vec3f operator-(const Vec3f& v1, const Vec3f& v2);

Vec3f operator*(float multiplier, Vec3f v);

#endif