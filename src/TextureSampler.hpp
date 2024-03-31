#pragma once

#include "forwards.hpp"
#include <tgaimage.h>


/**
 * @brief Holds a texture and provides getting color from it by normalized uv-coordinates.
 */
class TextureSampler
{
	static const Vec4 ERROR_COLOR;

public:
	TextureSampler() = default;

	explicit TextureSampler(const TGAImage& texture);

	/**
	 * @brief Get normalized color (0-1) at the given normalized texture coordinates.
	 */
	[[nodiscard]]
	Vec4 get(float u, float v) const;

	/**
	 * @brief Get normalized color (0-1) at the given normalized texture coordinates.
	 */
	[[nodiscard]]
	Vec4 get(const Vec2& uv) const;

private:
	TGAImage _texture;
};
