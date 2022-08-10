#include "Mat4.hpp"
#include "Utilities.hpp"
#include "Vec3f.hpp"
#include "Vec4.hpp"


Mat4::Mat4(
		float e00, float e01, float e02, float e03,
		float e10, float e11, float e12, float e13,
		float e20, float e21, float e22, float e23,
		float e30, float e31, float e32, float e33
)
{
	data = {
			Mat4::Row{e00, e01, e02, e03},
			Mat4::Row{e10, e11, e12, e13},
			Mat4::Row{e20, e21, e22, e23},
			Mat4::Row{e30, e31, e32, e33}
	};
}


Mat4::Mat4()
{
	data = {
			Mat4::Row{0, 0, 0, 0},
			Mat4::Row{0, 0, 0, 0},
			Mat4::Row{0, 0, 0, 0},
			Mat4::Row{0, 0, 0, 0}
	};
}


Mat4::Row& Mat4::operator[](std::size_t i)
{
	return data[i];
}


const Mat4::Row& Mat4::operator[](std::size_t i) const
{
	return data[i];
}


void Mat4::transpose()
{
	std::swap(data[0][1], data[1][0]);
	std::swap(data[0][2], data[2][0]);
	std::swap(data[0][3], data[3][0]);
	std::swap(data[1][2], data[2][1]);
	std::swap(data[1][3], data[3][1]);
	std::swap(data[2][3], data[3][2]);
}


Mat4 Mat4::getTransposed() const
{
	Mat4 copy = *this;
	copy.transpose();
	return copy;
}


Mat4 Mat4::getInverse() const
{
	// The formula was taken from the "Foundations of Game Engine Development" book, volume 1, chapter 1.7.

	const auto a = Vec3f(data[0][0], data[1][0], data[2][0]);
	const auto b = Vec3f(data[0][1], data[1][1], data[2][1]);
	const auto c = Vec3f(data[0][2], data[1][2], data[2][2]);
	const auto d = Vec3f(data[0][3], data[1][3], data[2][3]);

	const auto& x = data[3][0];
	const auto& y = data[3][1];
	const auto& z = data[3][2];
	const auto& w = data[3][3];

	Vec3f s = Vec3f::crossMultiply(a, b);
	Vec3f t = Vec3f::crossMultiply(c, d);
	Vec3f u = a * y - b * x;
	Vec3f v = c * w - d * z;

	const float det = Vec3f::dotMultiply(s, v) + Vec3f::dotMultiply(t, u);
	if (IsEqualFloat(det, 0.f)) {
		// The inverse doesn't exist!
		return {};
	}
	const float invDet = 1.f / det;

	s *= invDet;
	t *= invDet;
	u *= invDet;
	v *= invDet;

	const Vec3f r0 = Vec3f::crossMultiply(b, v) + t * y;
	const Vec3f r1 = Vec3f::crossMultiply(v, a) - t * x;
	const Vec3f r2 = Vec3f::crossMultiply(d, u) + s * w;
	const Vec3f r3 = Vec3f::crossMultiply(u, c) - s * z;

	const Mat4 inverse {
		r0.x, r0.y, r0.z, -Vec3f::dotMultiply(b, t),
		r1.x, r1.y, r1.z, Vec3f::dotMultiply(a, t),
		r2.x, r2.y, r2.z, -Vec3f::dotMultiply(d, s),
		r3.x, r3.y, r3.z, Vec3f::dotMultiply(c, s),
	};

	return inverse;
}


const Mat4& Mat4::identity()
{
	static const Mat4 identityMatrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	);
	return identityMatrix;
}


Vec4 operator*(const Mat4& m, const Vec4& v)
{
	Vec4 result(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
			m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
	);
	return result;
}


// Перемножение матрицы на вектор-столбец.
std::array<float, 4> operator*(const Mat4& m, const std::array<float, 4>& v)
{
	std::array<float, 4> result {
			m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3],
			m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
			m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3],
			m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3]
	};
	return result;
}


Mat4 operator*(const Mat4& m1, const Mat4& m2)
{
	const Mat4 m2Transposed = m2.getTransposed();
	Mat4 result;
	result[0] = m1 * m2Transposed[0];
	result[1] = m1 * m2Transposed[1];
	result[2] = m1 * m2Transposed[2];
	result[3] = m1 * m2Transposed[3];
	result.transpose();

	return result;
}


bool operator==(const Mat4& m1, const Mat4& m2)
{
	for (int row = 0; row < 4; ++row) {
		const auto& rowData1 = m1.data[row];
		const auto& rowData2 = m2.data[row];
		for (int column = 0; column < 4; ++column) {
			if (!IsEqualFloat(rowData1[column], rowData2[column])) {
				return false;
			}
		}
	}
	return true;
}
