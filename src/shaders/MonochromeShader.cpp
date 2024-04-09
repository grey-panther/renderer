//◦ Playrix ◦
#include "MonochromeShader.hpp"

// Include to get VertexData:
#include "DrawTools.hpp"


VertexData MonochromeShader::computeVertex(const VertexData& inVertexData) const
{
	VertexData outVertexData;

	// Apply transformations.
	outVertexData.position = transform * inVertexData.position;
	const Vec4 transformedNormal = normalsTransform * Vec4(inVertexData.normal, 0.f);
	outVertexData.normal = transformedNormal.xyz().normalize();

	return outVertexData;
}


std::pair<bool, Vec4> MonochromeShader::computeFragment(const VertexData& fragmentData) const
{
	// Calculate the light intensity on the fragment.
	// A normal vector may become a non-unit vector after fragment interpolation.
	const Vec3 normal = fragmentData.normal.normalized();
	float lightIntensity = Vec3::dotMultiply(normal, lightVector);
	if (lightIntensity < 0) {
		lightIntensity = 0.f;
	}

	// Shade the fragment with gray color according to the calculated light intensity.
	return {true, Vec4(lightIntensity, lightIntensity, lightIntensity, 1.f)};
}
