#pragma once

#include "forwards.hpp"


struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	explicit Vec4(float x = 0, float y = 0, float z = 0, float w = 0)
			: x(x)
			, y(y)
			, z(z)
			, w(w)
	{}

	Vec4(const Vec3& vec3, float w);

	Vec4& operator*=(float multiplier);

	Vec4& operator/=(float divider);

	Vec4& operator+=(float addition);

	Vec4& operator+=(const Vec4& other);

	Vec4& operator-=(const Vec4& other);

	Vec3 xyz() const;
};

Vec4 operator+(const Vec4& v1, const Vec4& v2);

Vec4 operator-(const Vec4& v1, const Vec4& v2);

Vec4 operator*(float multiplier, const Vec4& v);

Vec4 operator/(const Vec4& v, float divider);

bool operator==(const Vec4& v1, const Vec4& v2);

inline bool operator!=(const Vec4& v1, const Vec4& v2)
{
	return !(v1 == v2);
}
