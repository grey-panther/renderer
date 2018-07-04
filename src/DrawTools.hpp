#ifndef RENDERER_DRAW_TOOLS_H
#define RENDERER_DRAW_TOOLS_H

#include "Point.hpp"
#include "Vec3i.hpp"
#include <tgaimage.h>
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
		const Point p1,
		const Point p2,
		const Point p3,
		std::vector<int>& zBuffer,
		TGAImage& image,
		const TGAColor color
);

void drawTriangle(
		const Vec3i& p1,
		const Vec3i& p2,
		const Vec3i& p3,
		std::vector<int>& zBuffer,
		TGAImage& image,
		const TGAColor& color
);

#endif
