#include "NormalMapShader.hpp"

// Include to get VertexData:
#include "DrawTools.hpp"


VertexData NormalMapShader::computeVertex(const VertexData& inVertexData) const
{
	VertexData outVertexData;

	// Apply transformations.
	outVertexData.position = transform * inVertexData.position;
	outVertexData.textureCoords = inVertexData.textureCoords;

	return outVertexData;
}

std::pair<bool, Vec4> NormalMapShader::computeFragment(const VertexData& fragmentData) const
{
	// Get normal vector with its components in [0, 1]. The pixel format is RGB, so don't consider Alpha.
	const Vec3 modelNormalRgb = normalMap.get(fragmentData.textureCoords).rgb();
	// Transform components from [0, 1] to [-1, 1].
	const Vec3 modelNormal = modelNormalRgb * 2 - Vec3(1);
	// Transform the normal vector from model space to clip space.
	const Vec4 transformedNormal = normalsTransform * Vec4(modelNormal, 0.f);
	const Vec3 normal = transformedNormal.xyz().normalized();

	// Calculate the light intensity on the fragment.
	float lightIntensity = Vec3::dotMultiply(normal, lightVector);
	if (lightIntensity < 0) {
		lightIntensity = 0.f;
	}

	// Get the diffuse color.
	const Vec4 diffuseColor = texture.get(fragmentData.textureCoords);

	// Shade the color according to the calculated light intensity.
	const Vec3 rgb = diffuseColor.rgb() * lightIntensity;

	return {true, Vec4(rgb, diffuseColor.a)};
}
