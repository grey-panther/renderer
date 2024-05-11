#include "PhongReflectionShader.hpp"

// Include to get VertexData:
#include "DrawTools.hpp"


VertexData PhongReflectionShader::computeVertex(const VertexData& inVertexData) const
{
	VertexData outVertexData;

	// Apply transformations.
	outVertexData.position = transform * inVertexData.position;
	outVertexData.textureCoords = inVertexData.textureCoords;

	return outVertexData;
}

std::pair<bool, Vec4> PhongReflectionShader::computeFragment(const VertexData& fragmentData) const
{
	// Get normal vector with its components in [0, 1]. The pixel format is RGB, so don't consider Alpha.
	const Vec3 modelNormalRgb = normalMap.get(fragmentData.textureCoords).rgb();
	// Transform components from [0, 1] to [-1, 1].
	const Vec3 modelNormal = modelNormalRgb * 2 - Vec3(1);
	// Transform the normal vector from model space to clip space.
	const Vec4 transformedNormal = normalsTransform * Vec4(modelNormal, 0.f);
	const Vec3 normal = transformedNormal.xyz().normalized();

	//
	// Calculate the diffuse reflection.
	//
	const float diffuseIntensity = std::max(0.f, Vec3::dotMultiply(normal, lightVector));

	//
	// Calculate the specular reflection.
	//
	const Vec3 lightReflectionVector = 2 * Vec3::dotMultiply(lightVector, normal) * normal - lightVector;
	// cos of angle between view vector (0, 0, 1) and lightReflectionVector (the result of dot product):
	const float reflViewCos = lightReflectionVector.z;
	const float specularPower = specularMap.get(fragmentData.textureCoords).x * specularPowerMultiplier;
	const float specularIntensity = std::pow(std::max(0.f, reflViewCos), specularPower);

	const Vec4 diffuseColor = diffuseColorMap.get(fragmentData.textureCoords);
	const Vec3 resultColor =
			(ambientIntensity + diffuseRatio * diffuseIntensity) * diffuseColor.rgb()
			+ (specularRatio * specularIntensity) * lightColor;
	return {true, Vec4(resultColor, diffuseColor.a)};
}
