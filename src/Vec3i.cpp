#include "Vec3i.hpp"

Vec3i operator-(const Vec3i& v1, const Vec3i& v2)
{
	Vec3i result = v1;
	result -= v2;
	return result;
}

Vec3i operator+(const Vec3i& v1, const Vec3i& v2)
{
	Vec3i result = v1;
	result += v2;
	return result;
}

Vec3i operator*(Vec3i v, int multiplier)
{
	return v *= multiplier;
}

Vec3i operator*(int multiplier, Vec3i v)
{
	return v *= multiplier;
}

Vec3i operator*(Vec3i v, float multiplier)
{
	return v *= multiplier;
}

Vec3i operator*(float multiplier, Vec3i v)
{
	return v *= multiplier;
}