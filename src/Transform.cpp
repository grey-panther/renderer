#include "Transform.hpp"

#include "Vec3f.hpp"
#include "Mat4.hpp"
#include "Utilities.hpp"


namespace Transform
{
	Mat4 makeRotationX(float angleRadians)
	{
		const float c = std::cos(angleRadians);
		const float s = std::sin(angleRadians);
		const Mat4 m = {
			1, 0, 0 , 0,
			0, c, -s, 0,
			0, s, c , 0,
			0, 0, 0 , 1,
		};
		return m;
	}


	Mat4 makeRotationY(float angleRadians)
	{
		const float c = std::cos(angleRadians);
		const float s = std::sin(angleRadians);
		const Mat4 m = {
				c, 0, s, 0,
				0, 1, 0, 0,
				-s, 0, c, 0,
				0, 0, 0, 1,
		};
		return m;
	}


	Mat4 makeRotationZ(float angleRadians)
	{
		const float c = std::cos(angleRadians);
		const float s = std::sin(angleRadians);
		const Mat4 m = {
				c, -s, 0, 0,
				s, c, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1,
		};
		return m;
	}


	Mat4 makeRotation(float angleRadians, const Vec3f& axis)
	{
		assertTrue(isEqualFloat(axis.length(), 1.f));

		const auto& a = axis;
		const float c = std::cos(angleRadians);
		const float s = std::sin(angleRadians);
		const float d = 1 - c;
		const float x = d * a.x;
		const float y = d * a.y;
		const float z = d * a.z;
		const float axay = x * a.y;
		const float axaz = x * a.z;
		const float ayaz = y * a.z;
		const float sax = s * a.x;
		const float say = s * a.y;
		const float saz = s * a.z;
		const Mat4 m = {
				(x * a.x + c), (axay - saz), (axaz + say), 0,
				(axay + saz), (y * a.y + c), (ayaz - sax), 0,
				(axaz - say), (ayaz + sax), (z * a.z + c), 0,
				0, 0, 0, 1,
		};
		return m;
	}


	Mat4 makeScale(float scale)
	{
		return {
			scale, 0, 0, 0,
			0, scale, 0, 0,
			0, 0, scale, 0,
			0, 0, 0, 1
		};
	}


	Mat4 makeScale(float scale, const Vec3f& v)
	{
		assertTrue(isEqualFloat(v.length(), 1.f));

		const float s = scale - 1;
		const float s_vx = s * v.x;
		const float s_vy = s * v.y;
		const float s_vz = s * v.z;
		const float s_vx_vy = s_vx * v.y;
		const float s_vx_vz = s_vx * v.z;
		const float s_vy_vz = s_vy * v.z;

		return {
				(s_vx * v.x + 1)	, s_vx_vy			, s_vx_vz			, 0,
				s_vx_vy				, (s_vy * v.y + 1)	, s_vy_vz			, 0,
				s_vx_vz				, s_vy_vz			, (s_vz * v.z + 1)	, 0,
				0					, 0					, 0					, 1,
		};
	}


	Mat4 makeTranslation(const Vec3f& v)
	{
		Mat4 result = {
				1.f, 0.f, 0.f, v.x,
				0.f, 1.f, 0.f, v.y,
				0.f, 0.f, 1.f, v.z,
				0.f, 0.f, 0.f, 1.f
		};
		return result;
	}
}
