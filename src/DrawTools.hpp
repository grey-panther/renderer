#ifndef RENDERER_DRAW_TOOLS_H
#define RENDERER_DRAW_TOOLS_H

#include "Vec3f.hpp"
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
		const Vec3f& p1,
		const Vec3f& p2,
		const Vec3f& p3,
		std::vector<int>& zBuffer,
		TGAImage& image,
		const TGAColor& color
);

struct VertexData
{
	VertexData(const Vec3i& coords, const Vec3f& textureCoords)
			: coords(coords)
			, textureCoords(textureCoords)
	{}


	Vec3i coords;
	Vec3f textureCoords;
};

void drawTriangle(
		std::array<VertexData, 3> vertexes,
		std::vector<int>& zBuffer,
		TGAImage& image,
		float colorIntensity,
		const TGAImage& texture
);

#endif
