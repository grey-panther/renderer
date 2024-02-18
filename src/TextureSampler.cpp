#include "TextureSampler.hpp"

#include "Vec2.hpp"
#include <cmath>


const TGAColor TextureSampler::ERROR_COLOR = TGAColor(255, 0, 255, 255);


TextureSampler::TextureSampler(const TGAImage& texture)
		: _texture(texture)
{
}


TGAColor TextureSampler::get(float u, float v) const
{
	if (_texture.get_width() <= 0 || _texture.get_height() <= 0) {
		return ERROR_COLOR;
	}

	const float clampedU = std::clamp(u, 0.f, 1.f);
	const float clampedV = std::clamp(v, 0.f, 1.f);
	const int x = static_cast<int>(std::round(clampedU * static_cast<float>(_texture.get_width() - 1)));
	const int y = static_cast<int>(std::round(clampedV * static_cast<float>(_texture.get_height() - 1)));

	return _texture.get(x, y);
}


TGAColor TextureSampler::get(const Vec2& uv) const
{
	return get(uv.x, uv.y);
}
