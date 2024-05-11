#include "DrawTools.hpp"

#include "Mat4.hpp"
#include "shaders/IShader.hpp"
#include "Utilities.hpp"

#include <algorithm>
#include <array>
#include <limits>


ZBuffer makeZBuffer(const TGAImage& outImage)
{
	// Make z-buffer and initialize it with lowest values.
	const auto pixelsCount = static_cast<size_t>(outImage.get_width() * outImage.get_height());
	return std::vector<float>(pixelsCount, std::numeric_limits<float>::lowest());
}


// Рисует линию по алгоритму Брезенхэма с int вычислениями
void drawLine(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color)
{
	bool coordsSwapped = false;

	// Необходимо итерировать по координате с наибольшей длиной проекции, иначе линия будет прерывистой.
	if (abs(x1 - x0) < abs(y1 - y0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		coordsSwapped = true;
	}

	// Итерирование будет в порядке возрастания x.
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	const int dx = x1 - x0;
	const int dx2 = dx * 2;
	const bool increaseY = (y1 >= y0);
	const int dErrorY = 2 * std::abs(y1 - y0);
	int y = y0;
	int errorY = 0;

	for (int x = x0; x <= x1; ++x) {
		if (coordsSwapped) {
			image.set(y, x, color);
		} else {
			image.set(x, y, color);
		}

		errorY += dErrorY;
		if (errorY >= dx) {
			y += increaseY ? 1 : -1;

			errorY -= dx2;
		}
	}

//    const float t = 1.0f / (x1 - x0);
//    float err = 0;
//    int y = y0;
//    const bool increaseY = y1 > y0;
//    for (int x = x0; x <= x1; ++x) {
//        if (coordsSwapped) {
//            image.set(y, x, color);
//        } else {
//            image.set(x, y, color);
//        }
//
//        err += t * std::abs(y1 - y0);
//        while (err >= 0.5f) {
//            err -= 1.0f;
//            y += increaseY ? 1 : -1;
//        }
//    }
}


void drawLine(const Vec2i& p1, const Vec2i& p2, TGAImage& image, const TGAColor& color)
{
	drawLine(p1.x, p1.y, p2.x, p2.y, image, color);
}


// Итератор, выполняющий проход по дискретному отрезку от точки (x0, y0) до точки (x1, y1)
// На каждый update увеличивается X координата и пересчитывается Y
// x0 <= x1
// Если dy < dx, то итератор аналогичен проходу по всем точкам дискретного отрезка в алгоритме Брезенхэма,
// иначе линия по координате y будет прерывистой
class DrawLineIterator
{
private:
	int _maxX;
	int _dx;
	int _dx2;
	int _errorY;
	bool _increaseY;
	int _dErrorY;
	int _currentX;
	int _currentY;

public:
	DrawLineIterator(int x0, int y0, int x1, int y1)
	{
		// Важно: x0 <= x1
		_maxX = x1;

		_dx = x1 - x0;
		_dx2 = _dx * 2;

		_currentX = x0;
		_currentY = y0;

		_errorY = 0;
		_increaseY = (y1 >= y0);
		_dErrorY = 2 * std::abs(y1 - y0); // dErrorY > 0
	}


	int getX()
	{
		return _currentX;
	}


	int getValue()
	{
		return _currentY;
	}


	void update()
	{
		if (_currentX > _maxX || _dx <= 0) {
			return;
		}

		_currentX++;
		_errorY += _dErrorY;

		while (_errorY >= _dx) {
			_currentY += _increaseY ? 1 : -1;
			_errorY -= _dx2;
		}
	}
};


void sortByX(std::array<Vec3, 3>& points)
{
	// Отсортировать точки по x координате
	int pointsCount = 3;

	for (int i = 0; i < pointsCount - 1; i++) {
		int minPointIndex = i;

		for (int j = i + 1; j < pointsCount; j++) {
			if (points[j].x < points[minPointIndex].x) {
				minPointIndex = j;
			}
		}

		if (minPointIndex != i) {
			Vec3 buffPoint = points[i];
			points[i] = points[minPointIndex];
			points[minPointIndex] = buffPoint;
		}
	}
}


void drawTriangle(const Vec3& p1, const Vec3& p2, const Vec3& p3, ZBuffer& zBuffer, TGAImage& image,
				  const TGAColor& color)
{
	std::array<Vec3, 3> points = {p1, p2, p3};
	sortByX(points);

	int x0 = (int) points[0].x;
	int y0 = (int) points[0].y;
	int z0 = (int) points[0].z;
	int x1 = (int) points[1].x;
	int y1 = (int) points[1].y;
	int z1 = (int) points[1].z;
	int x2 = (int) points[2].x;
	int y2 = (int) points[2].y;
	int z2 = (int) points[2].z;

	DrawLineIterator commonLineIteratorY(x0, y0, x2, y2);
	DrawLineIterator commonLineIteratorZ(x0, z0, x2, z2);
	DrawLineIterator leftLineIteratorY(x0, y0, x1, y1);
	DrawLineIterator leftLineIteratorZ(x0, z0, x1, z1);
	DrawLineIterator rightLineIteratorY(x1, y1, x2, y2);
	DrawLineIterator rightLineIteratorZ(x1, z1, x2, z2);

	for (int x = x0; x <= x2; x++) {

		if (x < x1) {
			drawLineWithDepthMask(x, commonLineIteratorY.getValue(), commonLineIteratorZ.getValue(), x,
								  leftLineIteratorY.getValue(), leftLineIteratorZ.getValue(), zBuffer, image, color);
			leftLineIteratorY.update();
			leftLineIteratorZ.update();
		} else {
			drawLineWithDepthMask(x, commonLineIteratorY.getValue(), commonLineIteratorZ.getValue(), x,
								  rightLineIteratorY.getValue(), rightLineIteratorZ.getValue(), zBuffer, image, color);
			rightLineIteratorY.update();
			rightLineIteratorZ.update();
		}

		commonLineIteratorY.update();
		commonLineIteratorZ.update();
	}
}


// Рисует линию по алгоритму Брезенхэма с int вычислениями
// Не рисует точки, чья глубина больше указанной в массиве zBuffer
void drawLineWithDepthMask(int x0, int y0, int z0, int x1, int y1, int z1, ZBuffer& zBuffer, TGAImage& image,
						   const TGAColor& color)
{
	const int imageWidth = image.get_width();

	bool coordsSwapped = false;

	if (abs(x1 - x0) < abs(y1 - y0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		coordsSwapped = true;
	}

	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dx2 = dx * 2;

	int y = y0;
	int errorY = 0;
	bool increaseY = (y1 >= y0);
	int dErrorY = increaseY ? 2 * (y1 - y0) : 2 * (y0 - y1); // dErrorY > 0

	int z = z0;
	int errorZ = 0;
	bool increaseZ = (z1 >= z0);
	int dErrorZ = increaseZ ? 2 * (z1 - z0) : 2 * (z0 - z1); // dErrorZ > 0

	for (int x = x0; x <= x1; x++) {
		int drawingX = coordsSwapped ? y : x;
		int drawingY = coordsSwapped ? x : y;
		int drawingPixelIndex = drawingX + drawingY * imageWidth;

		if (z > zBuffer[drawingPixelIndex]) {
			zBuffer[drawingPixelIndex] = z;
			image.set(drawingX, drawingY, color);
		}

		errorY += dErrorY;
		if (errorY >= dx) {
			y += increaseY ? 1 : -1;

			errorY -= dx2;
		}

		errorZ += dErrorZ;
		if (errorZ >= dx) {
			z += increaseZ ? 1 : -1;

			errorZ -= dx2;
		}
	}
}


[[nodiscard]]
std::tuple<Vec3, bool> calculateBarycentricCoordinates(
		const Vec2i& pointA,
		const Vec2i& pointB,
		const Vec2i& pointC,
		const Vec2i& pointP
)
{
	const Vec2i ab = pointB - pointA;
	const Vec2i ac = pointC - pointA;
	const Vec2i pa = pointA - pointP;

	const auto v1 = Vec3(ab.x, ac.x, pa.x);
	const auto v2 = Vec3(ab.y, ac.y, pa.y);
	const Vec3 crossVec = Vec3::crossMultiply(v1, v2);

	if (isEqualFloat(crossVec.z, 0)) {
		// crossVec.z = v1.x * v2.y - v1.y * v2.x (from cross product definition)
		// crossVec.z = ab.x * ac.y - ac.x * ab.y (from v1 and v2 initialization)
		// if crossVec.z == 0 then vectors ab and ac are parallel:
		//		ab.x * ac.y == ac.x * ab.y
		//		ab.x / ab.y == ac.x / ac.y
		//		That means that both vectors have the same tilt (angle) +- 180deg.
		// The triangle ABC is degenerate, because vectors ab and ac are parallel.
		// It's impossible to compute barycentric coordinates for a degenerate triangle.
		return {{0, 0, 0}, false};
	}

	const float u = crossVec.x / crossVec.z;
	const float v = crossVec.y / crossVec.z;
	const float t = 1 - u - v;

	return {{t, u, v}, true};
}


[[nodiscard]]
std::tuple<Vec3, bool> calculateClipSpaceBarycentricCoordinates(
	const std::array<VertexData, 3>& triangleVertices,
	const Vec2i& point
)
{
	// Calculate barycentric position in screen space (after perspective division).
	const auto [screenBarPos, success] = calculateBarycentricCoordinates(
		triangleVertices[0].screenSpacePosition.xy(),
		triangleVertices[1].screenSpacePosition.xy(),
		triangleVertices[2].screenSpacePosition.xy(),
		point
	);
	if (!success) {
		return {{0, 0, 0}, false};
	}

	//
	// Calculate barycentric position in clip space (before perspective division).
	//

	// A, B, C - vertex coordinates in clip space
	// A', B', C' - the same vertex coordinates in screen space after perspective division.
	// P and P' - some point in clip space and screen space respectively.
	// Translate the barycentric coordinates in screen space (alpha', beta', gamma')
	// to the barycentric coordinates in clip space before perspective division (alpha, beta, gamma).
	//						[alpha']
	// P' = [A' B' C'] *	[beta' ]
	//						[gamma']
	// P' = P.xyz / P.w
	// A' = A.xyz / A.w
	// B' = ...
	// C' = ...
	//											[alpha']
	// P' = [A.xyz/A.w B.xyz/B.w C.xyz/C.w] *	[beta' ]
	//											[gamma']
	//							  [alpha' / A.w]
	// P' = [A.xyz B.xyz C.xyz] * [beta'  / B.w]
	//							  [gamma' / C.w]
	// P.xyz = P' * P.w
	//								 [alpha' / A.w]
	// P.xyz = [A.xyz B.xyz C.xyz] * [beta'  / B.w] * P.w
	//								 [gamma' / C.w]
	// The column vector in the right side is the barycentric coords in clip space before perspective division.
	// The sum of normalized barycentric coords is equal to 1:
	// (alpha'/A.w + beta'/B.w + gamma'/C.w) * P.w = 1
	// P.w = 1 / (alpha'/A.w + beta'/B.w + gamma'/C.w)
	// alpha = (alpha' / A.w) / (alpha'/A.w + beta'/B.w + gamma'/C.w)
	// beta = ...
	// gamma = ...

	const Vec4& A = triangleVertices[0].position;
	const Vec4& B = triangleVertices[1].position;
	const Vec4& C = triangleVertices[2].position;
	float alpha = screenBarPos.x / A.w;
	float beta = screenBarPos.y / B.w;
	float gamma = screenBarPos.z / C.w;
	const float sum = alpha + beta + gamma;
	alpha /= sum;
	beta /= sum;
	gamma /= sum;
	assertTrue(isEqualFloat(alpha + beta + gamma, 1.f));

	const auto clipBarPos = Vec3(alpha, beta, gamma);
	return {clipBarPos, true};
}


void drawTriangle(
		const std::array<VertexData, TRIANGLE_VERTICES_COUNT>& inVertices,
		const IShader& shader,
		ZBuffer& zBuffer,
		TGAImage& outImage
)
{
	// Compute vertices and perform the perspective division.
	std::array<VertexData, TRIANGLE_VERTICES_COUNT> vertices;
	for (int i = 0; i < inVertices.size(); ++i) {
		vertices[i] = shader.computeVertex(inVertices[i]);
		auto& vertex = vertices[i];

		// Transform coordinates from homogeneous to 3D-cartesian ones (make w == 1).
		vertex.screenSpacePosition = (vertex.position.xyz() / vertex.position.w).rounded();
	}

	// Sort the vertices in order of ascending X.
	std::sort(vertices.begin(), vertices.end(), [](const VertexData& v1, const VertexData& v2) {
		return v1.screenSpacePosition.x < v2.screenSpacePosition.x;
	});

	const Vec3i& v1 = vertices[0].screenSpacePosition;
	const Vec3i& v2 = vertices[1].screenSpacePosition;
	const Vec3i& v3 = vertices[2].screenSpacePosition;

	const Vec3i v1v2 = v2 - v1;
	const Vec3i v2v3 = v3 - v2;
	const Vec3i v1v3 = v3 - v1;

	// In order not to perform this division in a loop, calculate it beforehand.
	const float inverseV1V3X = 1.0f / v1v3.x;
	bool isLeft = true;

	for (int x = v1.x; x <= v3.x; ++x) {
		if (x == v2.x) {
			if (v2.x == v3.x) {
				// Exit in order to avoid division by 0 and an infinite loop.
				return;
			}
			// Start drawing the right part of the triangle from now on.
			isLeft = false;
		}

		// Determine the point on the vector v1v3 (the longest common edge of the triangle)
		// which corresponds to the current x.
		const float commonEdgeProgress = (x - v1.x) * inverseV1V3X;
		const Vec3i commonEdgePoint = v1 + commonEdgeProgress * v1v3;

		// Determine the point on the left (v1v2) or on the right (v2v3) short edge of the triangle.
		Vec3i shortEdgePoint;
		float leftEdgeProgress = 0.f;
		float rightEdgeProgress = 0.f;
		if (isLeft) {
			leftEdgeProgress = static_cast<float>(x - v1.x) / v1v2.x;
			shortEdgePoint = v1 + leftEdgeProgress * v1v2;
		}
		else {
			leftEdgeProgress = 1.f;
			rightEdgeProgress = static_cast<float>(x - v2.x) / v2v3.x;
			shortEdgePoint = v2 + rightEdgeProgress * v2v3;
		}

		const bool commonEdgeHigher = commonEdgePoint.y > shortEdgePoint.y;
		const Vec3i& vLow = commonEdgeHigher ? shortEdgePoint : commonEdgePoint;
		const Vec3i& vHigh = commonEdgeHigher ? commonEdgePoint : shortEdgePoint;

		int pixelIndex = vLow.y * outImage.get_width() + x;

		// Draw the vertical line segment from vLow.y to vHigh.y.
		for (int y = vLow.y; y <= vHigh.y; ++y, pixelIndex += outImage.get_width()) {

			// Discard the fragment if it occurs to be outside the image.
			if (pixelIndex < 0 || pixelIndex >= zBuffer.size()) {
				continue;
			}

			// Linear interpolation of the fragment attributes doesn't work properly after perspective division.
			// Barycentric coordinates in screen space are also unsuitable for such interpolations.
			// Instead, use barycentric coordinates in clip space (before perspective division).

			// Barycentric coordinates of screen point {x, y} within the triangle before perspective division.
			const auto [barycentricPos, success] = calculateClipSpaceBarycentricCoordinates(vertices, {x, y});
			if (!success) {
				continue;
			}

			const float z =
					vertices[0].position.z * barycentricPos.x
					+ vertices[1].position.z * barycentricPos.y
					+ vertices[2].position.z * barycentricPos.z;

			// Check by z-buffer whether we can draw the fragment (the pixel) or not.
			if (z <= zBuffer[pixelIndex]) {
				continue;
			}

			VertexData fragmentData;

			// Set the fragment position in the output image space.
			fragmentData.screenSpacePosition = Vec3i(x, y, static_cast<int>(std::round(z)));

			// Interpolate the fragment attributes.
			fragmentData.normal =
					vertices[0].normal * barycentricPos.x
					+ vertices[1].normal * barycentricPos.y
					+ vertices[2].normal * barycentricPos.z;
			fragmentData.textureCoords =
					vertices[0].textureCoords * barycentricPos.x
					+ vertices[1].textureCoords * barycentricPos.y
					+ vertices[2].textureCoords * barycentricPos.z;

			const auto& [needDraw, srcColor] = shader.computeFragment(fragmentData);
			if (needDraw) {
				// Alpha blending.
				// RGB_res = RGB_src * A_src + RGB_dst * (1 - A_src)
				// A_res = A_src * (1 - A_dst) + A_dst * 1
				TGAColor tgaDstColor = outImage.get(x, y);
				const Vec4 dstColor {tgaDstColor.r / 255.f, tgaDstColor.g / 255.f, tgaDstColor.b / 255.f, tgaDstColor.a / 255.f};
				Vec4 color;
				color.r = std::min(1.f, srcColor.r * srcColor.a + dstColor.r * (1.f - srcColor.a));
				color.g = std::min(1.f, srcColor.g * srcColor.a + dstColor.g * (1.f - srcColor.a));
				color.b = std::min(1.f, srcColor.b * srcColor.a + dstColor.b * (1.f - srcColor.a));
				color.a = std::min(1.f, srcColor.a * (1 - dstColor.a) + dstColor.a);

				TGAColor tgaColor;
				// Convert normalized 0-1 color channels to 0-255 byte channels.
				tgaColor.r = static_cast<unsigned char>(std::clamp(std::round(color.r * 255), 0.f, 255.f));
				tgaColor.g = static_cast<unsigned char>(std::clamp(std::round(color.g * 255), 0.f, 255.f));
				tgaColor.b = static_cast<unsigned char>(std::clamp(std::round(color.b * 255), 0.f, 255.f));
				tgaColor.a = static_cast<unsigned char>(std::clamp(std::round(color.a * 255), 0.f, 255.f));

				outImage.set(x, y, tgaColor);
				zBuffer[pixelIndex] = z;
			}
		}
	}
}
