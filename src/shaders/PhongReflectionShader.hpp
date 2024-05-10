#pragma once

#include "forwards.hpp"
#include "Mat4.hpp"
#include "Vec3.hpp"
#include "shaders/IShader.hpp"
#include "TextureSampler.hpp"


/**
 * The shader implements Phong Reflection Model (https://en.wikipedia.org/wiki/Phong_reflection_model).
 * It uses a combination of ambient light, diffuse reflection and specular reflection to calculate a fragment color.
 */
class PhongReflectionShader : public IShader
{
public:
	[[nodiscard]]
	VertexData computeVertex(const VertexData& inVertexData) const override;

	[[nodiscard]]
	std::pair<bool, Vec4> computeFragment(const VertexData& fragmentData) const override;

public:
	Mat4 transform;

	Mat4 normalsTransform;

	Vec3 lightVector;

	// The color of light source.
	Vec3 lightColor = {1.f, 1.f, 1.f};

	TextureSampler diffuseColorMap;

	TextureSampler normalMap;

	TextureSampler specularMap;

	// Ambient light intensity.
	float ambientIntensity = 0.3f;

	// Ratio of diffuse reflection.
	float diffuseRatio = 0.5f;

	// Ratio of specular reflection.
	float specularRatio = 0.5f;

	// Multiplier to a value got from the specular map.
	float specularPowerMultiplier = 20.f;
};
