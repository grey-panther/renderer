#include "SimpleLightShader.hpp"

// Include to get VertexData:
#include "DrawTools.hpp"
#include "Mat4.hpp"


VertexData SimpleLightShader::computeVertex(
		const VertexData& inVertexData
) const
{
	VertexData outVertexData;

	// Apply transformations.
	outVertexData.position = transform * inVertexData.position;
	const Vec4 transformedNormal = normalsTransform * Vec4(inVertexData.normal, 0.f);
	outVertexData.normal = transformedNormal.xyz().normalize();
	outVertexData.textureCoords = inVertexData.textureCoords;

	return outVertexData;
}


std::pair<bool, Vec4> SimpleLightShader::computeFragment(const VertexData& fragmentData) const
{
	// Calculate the light intensity on the fragment.
	// A normal vector may become a non-unit vector after fragment interpolation.
	const Vec3& normal = fragmentData.normal.normalized();
	float lightIntensity = Vec3::dotMultiply(normal, lightVector);
	if (lightIntensity < 0) {
		lightIntensity = 0.f;
	}

	// Get the color from the texture using the calculated uv-coordinates.
	const Vec4 textureColor = texture.get(fragmentData.textureCoords);

	// Shade the color according to the calculated light intensity.
	const Vec3 rgb = textureColor.rgb() * lightIntensity;

	return {true, Vec4(rgb, textureColor.a)};
}
