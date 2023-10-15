#include "Vec3.hpp"
#include "Vec3i.hpp"


Vec3& Vec3::operator*=(float multiplier)
{
	x *= multiplier;
	y *= multiplier;
	z *= multiplier;
	return *this;
}


Vec3& Vec3::operator/=(float divider)
{
	x /= divider;
	y /= divider;
	z /= divider;
	return *this;
}


Vec3& Vec3::operator+=(float addition)
{
	x += addition;
	y += addition;
	z += addition;
	return *this;
}


Vec3& Vec3::operator+=(const Vec3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}


Vec3& Vec3::operator-=(const Vec3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}


Vec3& Vec3::normalize()
{
	float length = this->length();
	if (length <= 0.0000001) {
		return *this;
	}

	float inverseLength = 1 / length;

	x *= inverseLength;
	y *= inverseLength;
	z *= inverseLength;

	return *this;
}


Vec3 Vec3::normalized() const
{
	auto v = *this;
	v.normalize();
	return v;
}


float Vec3::length() const
{
	return std::sqrt(x * x + y * y + z * z);
}


Vec3i Vec3::round() const
{
	return Vec3i(*this);
}


float Vec3::distance(const Vec3& p0, const Vec3& p1)
{
	return (p1 - p0).length();
}


Vec3 Vec3::crossMultiply(const Vec3& a, const Vec3& b)
{
	float iFactor = a.y * b.z - a.z * b.y;
	float jFactor = a.z * b.x - a.x * b.z;
	float kFactor = a.x * b.y - a.y * b.x;
	return Vec3(iFactor, jFactor, kFactor);
}


float Vec3::dotMultiply(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}


Vec3 operator+(const Vec3& v1, const Vec3& v2)
{
	Vec3 result = v1;
	result += v2;
	return result;
}


Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
	Vec3 result = v1;
	result -= v2;
	return result;
}


Vec3 operator*(float multiplier, Vec3 v)
{
	return v *= multiplier;
}


Vec3 operator/(const Vec3& v, float divider)
{
	Vec3 result = v;
	result /= divider;
	return result;
}
