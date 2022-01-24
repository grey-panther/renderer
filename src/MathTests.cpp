#include "MathTests.hpp"
#include "Mat4.hpp"
#include "Utilities.hpp"
#include "Vec4.hpp"
#include <iostream>


void testMat4Transpose()
{
	Mat4 m {
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15,
	};

	const Mat4 check {
		0, 4, 8, 12,
		1, 5, 9, 13,
		2, 6, 10, 14,
		3, 7, 11, 15,
	};

	assert(m.getTransposed() == check);

	m.transpose();
	assert(m == check);
}


void testMat4MultiplyVec4()
{
	const Mat4 m {
		5, 	7, 	9, 	10,
		2, 	3, 	3, 	8,
		8, 	10, 2, 	3,
		3, 	3, 	4, 	8,
	};
	const Vec4 v {
		3,
		12,
		9,
		3,
	};
	const Vec4 checkMul {
		210,
		93,
		171,
		105,
	};

	const Vec4 mul = m * v;
	assert(checkMul == mul);
}


void testMat4MultiplyMat4()
{
	const Mat4 m1 {
			5, 	7, 	9, 	10,
			2, 	3, 	3, 	8,
			8, 	10, 2, 	3,
			3, 	3, 	4, 	8,
	};
	const Mat4 m2 {
			3, 	10, 12, 18,
			12, 1, 	4, 	9,
			9, 	10, 12, 2,
			3, 	12, 4, 	10,
	};
	const Mat4 checkMul {
			210, 267, 236, 271,
			93,  149, 104, 149,
			171, 146, 172, 268,
			105, 169, 128, 169,
	};

	const Mat4 mul = m1 * m2;
	assert(checkMul == mul);
}


void testMat4()
{
	testMat4Transpose();
	testMat4MultiplyVec4();
	testMat4MultiplyMat4();
}
