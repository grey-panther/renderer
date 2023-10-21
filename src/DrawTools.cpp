#include "DrawTools.hpp"

#include "Mat4.hpp"

#include <algorithm>
#include <array>


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


void drawTriangle(const Vec3& p1, const Vec3& p2, const Vec3& p3, std::vector<int>& zBuffer, TGAImage& image,
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
void drawLineWithDepthMask(int x0, int y0, int z0, int x1, int y1, int z1, std::vector<int>& zBuffer, TGAImage& image,
						   const TGAColor color)
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


VertexData computeVertex(const VertexData& inVertexData, const Mat4& transform, const Mat4& normalsTransform)
{
	VertexData outVertexData;

	// Apply transformations.
	outVertexData.position = transform * inVertexData.position;
	const Vec4 transformedNormal = normalsTransform * Vec4(inVertexData.normal, 0.f);
	outVertexData.normal = transformedNormal.xyz().normalize();
	outVertexData.textureCoords = inVertexData.textureCoords;

	return outVertexData;
}


bool computeFragment(
		const VertexData& fragmentData,
		TGAImage& outImage,
		const Vec3& lightVector,
		const TGAImage& texture
)
{
	// Calculate the light intensity on the fragment.
	// A normal vector may become a non-unit vector after fragment interpolation.
	const Vec3& normal = fragmentData.normal.normalized();
	float lightIntensity = Vec3::dotMultiply(normal, lightVector);
	if (lightIntensity < 0) {
		lightIntensity = 0.f;
	}

	// Get the color from the texture using the calculated uv-coordinates.
	const Vec3 uv = fragmentData.textureCoords;
	const int u = static_cast<int>(std::round(uv.x * texture.get_width()));
	const int v = static_cast<int>(std::round(uv.y * texture.get_height()));
	const TGAColor textureColor = texture.get(u, v);

	// Shade the color according to the calculated light intensity.
	const auto r = static_cast<unsigned char>(textureColor.r * lightIntensity);
	const auto g = static_cast<unsigned char>(textureColor.g * lightIntensity);
	const auto b = static_cast<unsigned char>(textureColor.b * lightIntensity);

	outImage.set(fragmentData.screenSpacePosition.x, fragmentData.screenSpacePosition.y, TGAColor(r, g, b, 255));

	return true;
}


template<class VarType>
class ScreenSpaceFragmentVarInterpolator
{
public:
	ScreenSpaceFragmentVarInterpolator(const VarType& v1, const VarType& v2, const VarType& v3)
			: _v1(v1)
			, _v2(v2)
			, _v3(v3)
	{
	}


	VarType lerp(const VarType& from, const VarType& to, float t)
	{
		if (t <= 0.f) {
			return from;
		}
		if (t >= 1.f) {
			return to;
		}
		return from + t * (to - from);
	}


	VarType Calculate(
			float commonEdgeProgress,
			float leftEdgeProgress,
			float rightEdgeProgress,
			float verticalProgress
	)
	{
		const auto& v1 = _v1;
		const auto& v2 = _v2;
		const auto& v3 = _v3;

		const auto v1v2 = v2 - v1;
		const auto v2v3 = v3 - v2;
		const auto v1v3 = v3 - v1;

		// Determine the point on the vector v1v3 (the longest common edge of the triangle)
		// which corresponds to the current x.
		const auto vertSegmentEndValue1 = v1 + commonEdgeProgress * v1v3;

		// Determine the point on the left (v1v2) or on the right (v2v3) short edge of the triangle.
		VarType vertSegmentEndData2;
		const bool isLeft = (leftEdgeProgress < 1.f);
		if (isLeft) {
			vertSegmentEndData2 = v1 + leftEdgeProgress * v1v2;
		}
		else {
			vertSegmentEndData2 = v2 + rightEdgeProgress * v2v3;
		}

		const VarType result = lerp(vertSegmentEndValue1, vertSegmentEndData2, verticalProgress);
		return result;
	}

private:
	const VarType _v1;
	const VarType _v2;
	const VarType _v3;
};


void drawTriangle(
		std::array<VertexData, 3> vertices,
		std::vector<int>& zBuffer,
		TGAImage& outImage,
		const Vec3& lightVector,
		const TGAImage& texture
)
{
	// Sort the vertices in order of ascending X.
	std::sort(vertices.begin(), vertices.end(), [](const VertexData& v1, const VertexData& v2) {
		return v1.screenSpacePosition.x < v2.screenSpacePosition.x;
	});

	ScreenSpaceFragmentVarInterpolator<Vec3> normalInterp(
			vertices[0].normal,
			vertices[1].normal,
			vertices[2].normal
	);
	ScreenSpaceFragmentVarInterpolator<Vec3> textureInterp(
			vertices[0].textureCoords,
			vertices[1].textureCoords,
			vertices[2].textureCoords
	);

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

		const int yDiff = vHigh.y - vLow.y;
		const int zDiff = vHigh.z - vLow.z;
		int pixelIndex = vLow.y * outImage.get_width() + x;

		// Draw the vertical line segment from vLow.y to vHigh.y.
		for (int y = vLow.y; y <= vHigh.y; ++y, pixelIndex += outImage.get_width()) {

			// Discard the fragment if it occurs to be outside the image.
			if (pixelIndex < 0 || pixelIndex >= zBuffer.size()) {
				continue;
			}

			float verticalProgress = 0.f;
			int z = 0;
			if (yDiff == 0) {
				verticalProgress = 1.f;
				z = std::max(vLow.z, vHigh.z);
			} else {
				verticalProgress = static_cast<float>(y - vLow.y) / yDiff;
				z = vLow.z + static_cast<int>(std::round(verticalProgress * zDiff));
			}

			// Check by z-buffer whether we can draw the fragment (the pixel) or not.
			if (z > zBuffer[pixelIndex]) {
				VertexData fragmentData;
				// Set pixel coordinates in the output image space.
				fragmentData.screenSpacePosition = Vec3i(x, y, z);

				float fromCommonVertProgress = verticalProgress;
				if (commonEdgeHigher) {
					fromCommonVertProgress = 1.f - verticalProgress;
				}
				fragmentData.normal = normalInterp.Calculate(commonEdgeProgress, leftEdgeProgress, rightEdgeProgress, fromCommonVertProgress);
				fragmentData.textureCoords = textureInterp.Calculate(commonEdgeProgress, leftEdgeProgress, rightEdgeProgress, fromCommonVertProgress);

				const bool needUpdateZBuffer = computeFragment(fragmentData, outImage, lightVector, texture);
				if (needUpdateZBuffer) {
					zBuffer[pixelIndex] = z;
				}
			}
		}
	}
}
