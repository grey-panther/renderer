#pragma once

#include "forwards.hpp"
#include <tgaimage.h>


/**
 * @brief Holds a texture and provides getting color from it by normalized uv-coordinates.
 */
class TextureSampler
{
	static const TGAColor ERROR_COLOR;

public:
	TextureSampler() = default;

	explicit TextureSampler(const TGAImage& texture);

	[[nodiscard]]
	TGAColor get(float u, float v) const;

	[[nodiscard]]
	TGAColor get(const Vec2& uv) const;

private:
	TGAImage _texture;
};
