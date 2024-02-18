#include "Shader.hpp"

// Include to get VertexData:
#include "DrawTools.hpp"
#include "Mat4.hpp"


VertexData Shader::computeVertex(
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


bool Shader::computeFragment(
		const VertexData& fragmentData,
		TGAImage& outImage
) const
{
	// Calculate the light intensity on the fragment.
	// A normal vector may become a non-unit vector after fragment interpolation.
	const Vec3& normal = fragmentData.normal.normalized();
	float lightIntensity = Vec3::dotMultiply(normal, lightVector);
	if (lightIntensity < 0) {
		lightIntensity = 0.f;
	}

	// Get the color from the texture using the calculated uv-coordinates.
	const TGAColor textureColor = texture.get(fragmentData.textureCoords);

	// Shade the color according to the calculated light intensity.
	const auto r = static_cast<unsigned char>(textureColor.r * lightIntensity);
	const auto g = static_cast<unsigned char>(textureColor.g * lightIntensity);
	const auto b = static_cast<unsigned char>(textureColor.b * lightIntensity);

	outImage.set(fragmentData.screenSpacePosition.x, fragmentData.screenSpacePosition.y, TGAColor(r, g, b, 255));

	return true;
}
