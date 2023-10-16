#pragma once

#include "Vec3.hpp"
#include "Vec3i.hpp"
#include <tgaimage.h>
#include <array>
#include <vector>


void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color);

void drawLine(const Vec2i& p1, const Vec2i& p2, TGAImage& image, const TGAColor& color);

void drawLineWithDepthMask(
		int x0,
		int y0,
		int z0,
		int x1,
		int y1,
		int z1,
		std::vector<int>& zBuffer,
		TGAImage& image,
		const TGAColor color
);

// TODO Deprecated function.
void drawTriangle(
		const Vec3& p1,
		const Vec3& p2,
		const Vec3& p3,
		std::vector<int>& zBuffer,
		TGAImage& image,
		const TGAColor& color
);

struct VertexData
{
	VertexData(const Vec3i& coords, const Vec3& textureCoords, const Vec3& normal)
			: coords(coords)
			, textureCoords(textureCoords)
			, normal(normal)
	{}

	VertexData() = default;

public:
	Vec3i coords;
	Vec3 textureCoords;
	Vec3 normal;
};

void drawTriangle(
		std::array<VertexData, 3> vertices,
		std::vector<int>& zBuffer,
		TGAImage& outImage,
		const Vec3& lightVector,
		const TGAImage& texture
);
