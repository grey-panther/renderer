#pragma once

#include "Vec2i.hpp"
#include "Vec3.hpp"
#include "Vec3i.hpp"
#include "Vec4.hpp"
#include <tgaimage.h>
#include <array>
#include <vector>


using ZBuffer = std::vector<float>;

ZBuffer makeZBuffer(const TGAImage& outImage);

void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color);

void drawLine(const Vec2i& p1, const Vec2i& p2, TGAImage& image, const TGAColor& color);

void drawLineWithDepthMask(
		int x0,
		int y0,
		int z0,
		int x1,
		int y1,
		int z1,
		ZBuffer& zBuffer,
		TGAImage& image,
		const TGAColor color
);

// TODO Deprecated function.
void drawTriangle(
		const Vec3& p1,
		const Vec3& p2,
		const Vec3& p3,
		ZBuffer& zBuffer,
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
		ZBuffer& zBuffer,
		TGAImage& outImage,
		const Vec3& lightVector,
		const TGAImage& texture
);

/**
 * Calculates barycentric coordinates for the point P within the triangle ABC.
 *
 * @return The calculated coordinates and a success flag.
 * If the success flag is false, it indicates that the triangle ABC is degenerate
 * and the barycentric coordinates cannot be reliably calculated.
 */
[[nodiscard]]
std::tuple<Vec3, bool> calculateBarycentricCoordinates(
	const Vec2i& pointA,
	const Vec2i& pointB,
	const Vec2i& pointC,
	const Vec2i& pointP
);
