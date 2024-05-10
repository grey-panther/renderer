#include "TextureSampler.hpp"

#include "Vec2.hpp"
#include "Vec4.hpp"
#include <cmath>


const Vec4 TextureSampler::ERROR_COLOR = Vec4(1.f, 0, 1.f, 1.f);


TextureSampler::TextureSampler(const TGAImage& texture)
		: _texture(texture)
{
}


Vec4 TextureSampler::get(float u, float v) const
{
	if (_texture.get_width() <= 0 || _texture.get_height() <= 0) {
		return ERROR_COLOR;
	}

	const float clampedU = std::clamp(u, 0.f, 1.f);
	const float clampedV = std::clamp(v, 0.f, 1.f);
	const int x = static_cast<int>(std::round(clampedU * static_cast<float>(_texture.get_width() - 1)));
	const int y = static_cast<int>(std::round(clampedV * static_cast<float>(_texture.get_height() - 1)));

	const TGAColor tgaColor = _texture.get(x, y);
	switch (tgaColor.bytespp) {
		case TGAImage::Format::RGBA:
			return Vec4(tgaColor.r / 255.f, tgaColor.g / 255.f, tgaColor.b / 255.f, tgaColor.a / 255.f);
		case TGAImage::Format::RGB:
			return Vec4(tgaColor.r / 255.f, tgaColor.g / 255.f, tgaColor.b / 255.f, 1.f);
		case TGAImage::Format::GRAYSCALE: {
			const float normVal = tgaColor.val / 255.f;
			return Vec4(normVal, normVal, normVal, 1.f);
		}
		default:
			return ERROR_COLOR;
	}
}


Vec4 TextureSampler::get(const Vec2& uv) const
{
	return get(uv.x, uv.y);
}
