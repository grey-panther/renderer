#include "MathTests.hpp"
#include "Mat4.hpp"
#include "Transform.hpp"
#include "Utilities.hpp"
#include "Vec3f.hpp"
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


void testMat4Inverse1()
{
	const Mat4 m {
			1, 2, -1, 1,
			-1, 1, 2, -1,
			2, -1, 2, 2,
			1, 1, -1, 2,
	};

	const Mat4 inverse {
		15 / 17.f	, -7 / 17.f	, 6 / 17.f	, -1,
		6 / 17.f	, 4 / 17.f	, -1 / 17.f	, 0,
		-1 / 17.f	, 5 / 17.f	, 3 / 17.f	, 0,
		-11 / 17.f	, 4 / 17.f	, -1 / 17.f	, 1
	};
	const auto mul = m * inverse;
	assert(mul == Mat4::identity());

	const Mat4 calculatedInverse = m.getInverse();
	assert(calculatedInverse == inverse);
}


void testMat4Inverse2()
{
	const Mat4 m {
			6, -5, 8, 4,
			9, 7, 5, 2,
			7, 5, 3, 7,
			-4, 8, -8, -3,
	};

	const Mat4 inverse {
		139 / 25.f	, -77 / 100.f	, -93 / 100.f	, 473 / 100.f,
		-3			, 1 / 2.f		, 1 / 2.f		, -5 / 2.f,
		-134 / 25.f	, 87 / 100.f	, 83 / 100.f	, -463 / 100.f,
		-28 / 25.f	, 1 / 25.f		, 9 / 25.f		, -24 / 25.f
	};
	const auto mul = m * inverse;
	assert(mul == Mat4::identity());

	const Mat4 calculatedInverse = m.getInverse();
	assert(calculatedInverse == inverse);
}


void testMat4Inverse3()
{
	// Singular matrix (it doesn't have the inverse):
	const Mat4 m {
			6, -5, 8, 0,
			9, 7, 5, 0,
			7, 5, 3, 0,
			-4, 8, -8, 0,
	};

	// If the determinant is zero, the inverse doesn't exist and the function returns a zero matrix.
	const Mat4 calculatedInverse = m.getInverse();
	assert(calculatedInverse == Mat4());
}


void testMat4()
{
	testMat4Transpose();
	testMat4MultiplyVec4();
	testMat4MultiplyMat4();
	testMat4Inverse1();
	testMat4Inverse2();
	testMat4Inverse3();
}


void testMakeRotation()
{
	const float angleStep = PI / 3;
	for (float angle = 0; angle <= 2 * PI; angle += angleStep) {
		assert(Transform::makeRotationX(angle) == Transform::makeRotation(angle, Vec3f(1, 0, 0)));
		assert(Transform::makeRotationY(angle) == Transform::makeRotation(angle, Vec3f(0, 1, 0)));
		assert(Transform::makeRotationZ(angle) == Transform::makeRotation(angle, Vec3f(0, 0, 1)));
	}

	const float angle = PI / 5;
	assert((Transform::makeRotationX(-angle) * Transform::makeRotationX(angle)) == Mat4::identity());
	assert((Transform::makeRotationY(-angle) * Transform::makeRotationY(angle)) == Mat4::identity());
	assert((Transform::makeRotationZ(-angle) * Transform::makeRotationZ(angle)) == Mat4::identity());
}


void testMakeScale()
{
	const float scale = 42;
	const auto vector = Vec4(2, 3, 4, 0);
	const auto point = Vec4(-2, -3, -4, 1);

	// uniform scale
	{
		const Mat4 scaleTransform = Transform::makeScale(scale);

		const auto&& scaledVector = scaleTransform * vector;
		assert(scaledVector == Vec4(vector.x * scale, vector.y * scale, vector.z * scale, 0))

		const auto&& scaledPoint = scaleTransform * point;
		assert(scaledPoint == Vec4(point.x * scale, point.y * scale, point.z * scale, 1));

		const Mat4 revScaleTransform = Transform::makeScale(1 / scale);
		assert((scaleTransform * revScaleTransform) == Mat4::identity());

		assert((Transform::makeScale(1.f) * vector) == vector);
		assert((Transform::makeScale(1.f) * point) == point);
	}

	// scale along the arbitrary axis
	{
		// scale along x, y, z axes.
		{
			auto xScale = Transform::makeScale(scale, Vec3f(1, 0, 0));
			assert((xScale * vector) == Vec4(vector.x * scale, vector.y, vector.z, vector.w));
			assert((xScale * point) == Vec4(point.x * scale, point.y, point.z, point.w));

			auto yScale = Transform::makeScale(scale, Vec3f(0, 1, 0));
			assert((yScale * vector) == Vec4(vector.x, vector.y * scale, vector.z, vector.w));
			assert((yScale * point) == Vec4(point.x, point.y * scale, point.z, point.w));

			auto zScale = Transform::makeScale(scale, Vec3f(0, 0, 1));
			assert((zScale * vector) == Vec4(vector.x, vector.y, vector.z * scale, vector.w));
			assert((zScale * point) == Vec4(point.x, point.y, point.z * scale, point.w));
		}

		// scale along the unit vector
		{
			const auto dir = Vec3f(1.f / std::sqrt(2.f), 1.f / std::sqrt(2.f), 0.f);
			const float s = 3.f;
			auto&& dirScale = Transform::makeScale(s, dir);
			assert(dirScale == Mat4(
					2, 1, 0, 0,
					1, 2, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1
			));
			assert((dirScale * Vec4(2, 3, 4, 0)) == Vec4(7, 8, 4, 0));
			assert((dirScale * Vec4(2, 3, 4, 1)) == Vec4(7, 8, 4, 1));
		}
	}
}


void testTransforms()
{
	testMakeRotation();
	testMakeScale();
}
