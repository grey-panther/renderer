#include "Vec3f.hpp"
#include "Vec3i.hpp"


Vec3f& Vec3f::operator*=(int multiplier)
{
	x *= multiplier;
	y *= multiplier;
	z *= multiplier;
	return *this;
}


Vec3f& Vec3f::operator+=(int addition)
{
	x += addition;
	y += addition;
	z += addition;
	return *this;
}


Vec3f& Vec3f::operator+=(const Vec3f& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}


Vec3f& Vec3f::operator-=(const Vec3f& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}


Vec3f& Vec3f::normalize()
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


float Vec3f::length() const
{
	return std::sqrt(x * x + y * y + z * z);
}


Vec3i Vec3f::round() const
{
	return Vec3i(*this);
}


float Vec3f::distance(const Vec3f& p0, const Vec3f& p1)
{
	return (p1 - p0).length();
}


Vec3f Vec3f::crossMultiply(const Vec3f& a, const Vec3f& b)
{
	float iFactor = a.y * b.z - a.z * b.y;
	float jFactor = a.z * b.x - a.x * b.z;
	float kFactor = a.x * b.y - a.y * b.x;
	return Vec3f(iFactor, jFactor, kFactor);
}


float Vec3f::dotMultiply(const Vec3f& a, const Vec3f& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}


Vec3f operator+(const Vec3f& v1, const Vec3f& v2)
{
	Vec3f result = v1;
	result += v2;
	return result;
}


Vec3f operator-(const Vec3f& v1, const Vec3f& v2)
{
	Vec3f result = v1;
	result -= v2;
	return result;
}
