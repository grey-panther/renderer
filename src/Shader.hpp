#pragma once

#include "forwards.hpp"
#include "Mat4.hpp"
#include "Vec3.hpp"
#include "TextureSampler.hpp"
#include <tgaimage.h>


class VertexData;

class Shader
{
public:
	[[nodiscard]]
	VertexData computeVertex(const VertexData& inVertexData) const;

	/**
	 * @brief Compute the fragment and return its color.
	 * @return [needDraw, color]
	 */
	[[nodiscard]]
	std::pair<bool, Vec4> computeFragment(const VertexData& fragmentData) const;

public:
	Mat4 transform;

	Mat4 normalsTransform;

	Vec3 lightVector;

	TextureSampler texture;
};
