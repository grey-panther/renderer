#pragma once

#include "forwards.hpp"
#include "Mat4.hpp"
#include "Vec3.hpp"
#include "shaders/IShader.hpp"


class MonochromeShader : public IShader
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
};
