#include "Mat4.hpp"
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


Mat4::Row& Mat4::operator[](size_t i)
{
	return data[i];
}


const Mat4::Row& Mat4::operator[](size_t i) const
{
	return data[i];
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
