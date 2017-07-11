#ifndef POINT_DEFINED
#define POINT_DEFINED

#include <math.h>

struct Point
{
	double x, y, z;

	Point(double x, double y, double z)
			: x(x)
			, y(y)
			, z(z) {
	}

	// Возвращает новую точку, каждая координата которой умножена на скаляр
	Point mult(int multiplier) {
		return Point(x * multiplier, y * multiplier, z * multiplier);
	}

	// Возвращает новую точку, к каждой координате которой прибавлен скаляр
	Point add(int addition) {
		return Point(x + addition, y + addition, z + addition);
	}

	Point deduct(Point other) {
		return Point(x - other.x, y - other.y, z - other.z);
	}

	Point normalize() {
		double length = this->length();
		if (length <= 0.0000001) {
			return *this;
		}

		double inverseLength = 1 / length;

		x *= inverseLength;
		y *= inverseLength;
		z *= inverseLength;

		return *this;
	}

	double length() {
		return (double) sqrt(x * x + y * y + z * z);
	}

	static double distance(Point p0, Point p1) {
		double dx = p1.x - p0.x;
		double dy = p1.y - p0.y;
		double dz = p1.z - p0.z;
		return (double) sqrt(dx * dx + dy * dy + dz * dz);
	}

	static Point crossMultiply(Point a, Point b) {
		double iFactor = a.y * b.z - a.z * b.y;
		double jFactor = a.z * b.x - a.x * b.z;
		double kFactor = a.x * b.y - a.y * b.x;
		return Point(iFactor, jFactor, kFactor);
	}

	static double dotMultiply(Point a, Point b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
};

#endif