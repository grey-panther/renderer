#pragma once

#include <array>
#include "forwards.hpp"


struct Mat4
{
public:
	using Row = std::array<float, 4>;
	std::array<Row, 4> data;

public:
	Mat4(
			float e00, float e01, float e02, float e03,
			float e10, float e11, float e12, float e13,
			float e20, float e21, float e22, float e23,
			float e30, float e31, float e32, float e33
	);

	Mat4();

	Row& operator[](size_t i);

	const Row& operator[](size_t i) const;

	/**
	 * @brief Получить единичную матрицу.
	 */
	static const Mat4& identity();
};

Vec4 operator*(const Mat4& m, const Vec4& v);
