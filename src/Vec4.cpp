#include "Vec4.hpp"
#include "Vec3f.hpp"


Vec4::Vec4(const Vec3f& v, float w)
		: x(v.x)
		, y(v.y)
		, z(v.z)
		, w(w)
{
}


Vec4& Vec4::operator*=(float multiplier)
{
	x *= multiplier;
	y *= multiplier;
	z *= multiplier;
	w *= multiplier;
	return *this;
}


Vec4& Vec4::operator/=(float divider)
{
	x /= divider;
	y /= divider;
	z /= divider;
	w /= divider;
	return *this;
}


Vec4& Vec4::operator+=(float addition)
{
	x += addition;
	y += addition;
	z += addition;
	w += addition;
	return *this;
}


Vec4& Vec4::operator+=(const Vec4& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}


Vec4& Vec4::operator-=(const Vec4& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}


Vec3f Vec4::xyz() const
{
	return Vec3f(x, y, z);
}


Vec4 operator+(const Vec4& v1, const Vec4& v2)
{
	Vec4 result = v1;
	result += v2;
	return result;
}


Vec4 operator-(const Vec4& v1, const Vec4& v2)
{
	Vec4 result = v1;
	result -= v2;
	return result;
}


Vec4 operator*(float multiplier, const Vec4& v)
{
	Vec4 result = v;
	result *= multiplier;
	return result;
}


Vec4 operator/(const Vec4& v, float divider)
{
	Vec4 result = v;
	result /= divider;
	return result;
}
