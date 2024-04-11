#include "ToonShader.hpp"

// Include to get VertexData:
#include "DrawTools.hpp"
#include "Mat4.hpp"


VertexData ToonShader::computeVertex(const VertexData& inVertexData) const
{
	VertexData outVertexData;

	// Apply transformations.
	outVertexData.position = transform * inVertexData.position;
	const Vec4 transformedNormal = normalsTransform * Vec4(inVertexData.normal, 0.f);
	outVertexData.normal = transformedNormal.xyz().normalize();

	return outVertexData;
}


std::pair<bool, Vec4> ToonShader::computeFragment(const VertexData& fragmentData) const
{
	// Calculate the light intensity on the fragment.
	// A normal vector may become a non-unit vector after fragment interpolation.
	const Vec3& normal = fragmentData.normal.normalized();
	float light = Vec3::dotMultiply(normal, lightVector);

	if (light <= 0.001f) {
		light = 0.f;
	}
	else if (light < 0.4f) {
		light = 0.25f;
	}
	else if (light < 0.6f) {
		light = 0.45f;
	}
	else if (light < 0.8f) {
		light = 0.7f;
	}
	else if (light < 0.95f) {
		light = 0.88f;
	}
	else {
		light = 1.f;
	}

	const Vec4 baseColor = Vec4(0.f, 1.f, 0.f, 1.f);

	// Shade the color according to the calculated light intensity.
	const auto r = baseColor.x * light;
	const auto g = baseColor.y * light;
	const auto b = baseColor.z * light;

	return {true, Vec4(r, g, b, 1.f)};
}
