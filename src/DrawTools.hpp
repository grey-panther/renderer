#pragma once

#include "Vec3.hpp"
#include "Vec3i.hpp"
#include "Vec4.hpp"
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
	VertexData() = default;

public:
	Vec4 position;
	Vec3i screenSpacePosition;
	Vec3 textureCoords;
	Vec3 normal;
};

VertexData computeVertex(const VertexData& inVertexData, const Mat4& transform, const Mat4& normalsTransform);

// Return true if the fragment was drawn.
// Return false if the fragment wasn't drawn and should not affect z-buffer.
bool computeFragment(
		const VertexData& fragmentData,
		TGAImage& outImage,
		const Vec3& lightVector,
		const TGAImage& texture
);

void drawTriangle(
		std::array<VertexData, 3> vertices,
		std::vector<int>& zBuffer,
		TGAImage& outImage,
		const Vec3& lightVector,
		const TGAImage& texture
);
