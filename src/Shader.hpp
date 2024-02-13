#pragma once

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
	 * @brief Compute the fragment color and put it to the outImage.
	 * @return true if the fragment was drawn; false if the fragment wasn't drawn and should not affect z-buffer.
	 */
	bool computeFragment(const VertexData& fragmentData, TGAImage& outImage) const;

public:
	Mat4 transform;

	Mat4 normalsTransform;

	Vec3 lightVector;

	TextureSampler texture;
};
