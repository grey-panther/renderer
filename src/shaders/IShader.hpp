#pragma once

#include "forwards.hpp"
#include "TextureSampler.hpp"


class VertexData;

/**
 * Interface for a shader implementation.
 */
class IShader
{
public:
	virtual ~IShader() noexcept = default;

	[[nodiscard]]
	virtual VertexData computeVertex(const VertexData& inVertexData) const = 0;

	/**
	 * @brief Compute the fragment and return its color.
	 * @return [needDraw, color]
	 */
	[[nodiscard]]
	virtual std::pair<bool, Vec4> computeFragment(const VertexData& fragmentData) const = 0;
};
