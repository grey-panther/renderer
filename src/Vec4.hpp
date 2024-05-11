#pragma once

#include "forwards.hpp"


struct Vec4
{
public:
	Vec4() = default;

	explicit Vec4(float scalar);

	Vec4(float x, float y, float z, float w);

	Vec4(const Vec3& vec3, float w);

	Vec4& operator*=(float multiplier);

	Vec4& operator/=(float divider);

	Vec4& operator+=(float addition);

	Vec4& operator+=(const Vec4& other);

	Vec4& operator-=(const Vec4& other);

	[[nodiscard]]
	Vec3 xyz() const;

	[[nodiscard]]
	Vec3 rgb() const;

public:
	union {
		float x = 0.f;
		float r;
	};

	union {
		float y = 0.f;
		float g;
	};

	union {
		float z = 0.f;
		float b;
	};

	union {
		float w = 0.f;
		float a;
	};
};

Vec4 operator+(const Vec4& v1, const Vec4& v2);

Vec4 operator-(const Vec4& v1, const Vec4& v2);

Vec4 operator*(float multiplier, const Vec4& v);

[[nodiscard]]
inline Vec4 operator*(const Vec4& v, float multiplier) { return operator*(multiplier, v); }

Vec4 operator/(const Vec4& v, float divider);

bool operator==(const Vec4& v1, const Vec4& v2);

inline bool operator!=(const Vec4& v1, const Vec4& v2)
{
	return !(v1 == v2);
}
