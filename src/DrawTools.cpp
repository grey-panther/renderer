#include "DrawTools.hpp"

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


// Return true if the fragment was drawn.
// Return false if the fragment wasn't drawn and should not affect z-buffer.
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
	const float lightIntensity = Vec3::dotMultiply(normal, lightVector);

	if (lightIntensity < 0) {
		// Discard an invisible fragment.
		// The discarded fragment won't be set in z-buffer so as not to overlap another,
		// possibly more distant, but visible fragment.
		return false;
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

	outImage.set(fragmentData.coords.x, fragmentData.coords.y, TGAColor(r, g, b, 255));

	return true;
}


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
		return v1.coords.x < v2.coords.x;
	});

	const Vec3i& v1 = vertices[0].coords;
	const Vec3i& v2 = vertices[1].coords;
	const Vec3i& v3 = vertices[2].coords;

	const Vec3i v1v2 = v2 - v1;
	const Vec3i v2v3 = v3 - v2;
	const Vec3i v1v3 = v3 - v1;

	const Vec3& vt1 = vertices[0].textureCoords;
	const Vec3& vt2 = vertices[1].textureCoords;
	const Vec3& vt3 = vertices[2].textureCoords;

	const Vec3 vt1vt2 = vt2 - vt1;
	const Vec3 vt2vt3 = vt3 - vt2;
	const Vec3 vt1vt3 = vt3 - vt1;

	const Vec3& vn1 = vertices[0].normal;
	const Vec3& vn2 = vertices[1].normal;
	const Vec3& vn3 = vertices[2].normal;

	const Vec3 vn1vn2 = vn2 - vn1;
	const Vec3 vn2vn3 = vn3 - vn2;
	const Vec3 vn1vn3 = vn3 - vn1;

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
		const float commonEdgeProgress = (x - v1.x) * inverseV1V3X;
		// Determine the point on the vector v1v3 (the longest common edge of the triangle)
		// which corresponds to the current x.
		VertexData drawingVertex1(
				v1 + commonEdgeProgress * v1v3,
				vt1 + commonEdgeProgress * vt1vt3,
				vn1 + commonEdgeProgress * vn1vn3
		);

		// Determine the point on the left (v1v2) or on the right (v2v3) short edge of the triangle.
		Vec3i v;
		Vec3 vt;
		Vec3 vn;
		if (isLeft) {
			const float leftEdgeProgress = static_cast<float>(x - v1.x) / v1v2.x;
			v = v1 + leftEdgeProgress * v1v2;
			vt = vt1 + leftEdgeProgress * vt1vt2;
			vn = vn1 + leftEdgeProgress * vn1vn2;
		}
		else {
			const float rightEdgeProgress = static_cast<float>(x - v2.x) / v2v3.x;
			v = v2 + rightEdgeProgress * v2v3;
			vt = vt2 + rightEdgeProgress * vt2vt3;
			vn = vn2 + rightEdgeProgress * vn2vn3;
		}
		VertexData drawingVertex2(v, vt, vn);

		// Let the second vertex have a bigger Y-coordinate than the first.
		if (drawingVertex1.coords.y > drawingVertex2.coords.y) {
			std::swap(drawingVertex1, drawingVertex2);
		}
		// Draw a vertical line segment from vLow.y to vHigh.y.
		const Vec3i& vLow = drawingVertex1.coords;
		const Vec3i& vHigh = drawingVertex2.coords;
		const Vec3& uv1 = drawingVertex1.textureCoords;
		const Vec3& uv2 = drawingVertex2.textureCoords;
		const Vec3& n1 = drawingVertex1.normal;
		const Vec3& n2 = drawingVertex2.normal;
		const int yDiff = vHigh.y - vLow.y;
		const int zDiff = vHigh.z - vLow.z;
		int pixelIndex = vLow.y * outImage.get_width() + x;

		for (int y = vLow.y; y <= vHigh.y; ++y, pixelIndex += outImage.get_width()) {

			// Discard the fragment if it occurs to be outside the image.
			if (pixelIndex < 0 || pixelIndex >= zBuffer.size()) {
				continue;
			}

			int z = 0;
			float t = 0.f;
			if (yDiff == 0) {
				t = 1.f;
				z = std::max(vLow.z, vHigh.z);
			} else {
				t = static_cast<float>(y - vLow.y) / yDiff;
				z = vLow.z + static_cast<int>(std::round(t * zDiff));
			}

			// Check by z-buffer whether we can draw the fragment (the pixel) or not.
			if (z > zBuffer[pixelIndex]) {
				VertexData fragmentData;
				// Set pixel coordinates in the output image space.
				fragmentData.coords = Vec3i(x, y, z);
				// Calculate the normal vector for the current fragment.
				fragmentData.normal = n1 + t * (n2 - n1);
				// Calculate uv-coordinates for the current fragment in the [0; 1] interval.
				fragmentData.textureCoords = uv1 + t * (uv2 - uv1);

				const bool needUpdateZBuffer = computeFragment(fragmentData, outImage, lightVector, texture);
				if (needUpdateZBuffer) {
					zBuffer[pixelIndex] = z;
				}
			}
		}
	}
}
