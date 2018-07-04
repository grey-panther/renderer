#ifndef POINT_DEFINED
#define POINT_DEFINED

#include <cmath>


// TODO Зарефакторить класс. Например, сделать .cpp файл и переименовать класс в Vec3f.
struct Point
{
	double x, y, z;

	Point(double x, double y, double z)
			: x(x)
			, y(y)
			, z(z) {
	}

	Point& mult(int multiplier) {
		x *= multiplier;
		y *= multiplier;
		z *= multiplier;
		return *this;
	}

	Point& add(int addition) {
		x += addition;
		y += addition;
		z += addition;
		return *this;
	}

	Point& deduct(const Point& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Point& normalize() {
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
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	/**
	 * @brief Векторное произведение.
	 */
	static Point crossMultiply(Point a, Point b) {
		double iFactor = a.y * b.z - a.z * b.y;
		double jFactor = a.z * b.x - a.x * b.z;
		double kFactor = a.x * b.y - a.y * b.x;
		return Point(iFactor, jFactor, kFactor);
	}

	/**
	 * @brief Скалярное произведение.
	 */
	static double dotMultiply(Point a, Point b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	static Point deduct(Point a, Point b) {
		return Point(a.x - b.x, a.y - b.y, a.z - b.z);
	}
};

#endif