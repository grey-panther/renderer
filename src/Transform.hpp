#pragma once

#include "forwards.hpp"


namespace Transform
{
	[[nodiscard]]
	Mat4 makeRotationX(float angleRadians);

	[[nodiscard]]
	Mat4 makeRotationY(float angleRadians);

	[[nodiscard]]
	Mat4 makeRotationZ(float angleRadians);

	[[nodiscard]]
	Mat4 makeRotation(float angleRadians, const Vec3& axis);

	// Make uniform scale.
	[[nodiscard]]
	Mat4 makeScale(float scale);

	// Make scale along unit-vector v.
	[[nodiscard]]
	Mat4 makeScale(float scale, const Vec3& v);

	[[nodiscard]]
	Mat4 makeTranslation(const Vec3& v);
}
