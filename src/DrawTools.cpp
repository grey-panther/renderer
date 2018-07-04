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


void sortByX(std::array<Point, 3>& points)
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
			Point buffPoint = points[i];
			points[i] = points[minPointIndex];
			points[minPointIndex] = buffPoint;
		}
	}
}


void drawTriangle(const Point p1, const Point p2, const Point p3, std::vector<int>& zBuffer, TGAImage& image,
				  const TGAColor color)
{
	std::array<Point, 3> points = {p1, p2, p3};
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


void drawTriangle(const Vec3i& p1, const Vec3i& p2, const Vec3i& p3, std::vector<int>& zBuffer, TGAImage& image,
				  const TGAColor& color)
{
	// Отсортировать полученные точки в порядке возрастания x.
	std::array<Vec3i, 3> points = {p1, p2, p3};
	std::sort(points.begin(), points.end(), [](const Vec3i& v1, const Vec3i& v2) { return v1.x < v2.x; });

	const Vec3i& v1 = points[0];
	const Vec3i& v2 = points[1];
	const Vec3i& v3 = points[2];

	const Vec3i v1v2 = v2 - v1;
	const Vec3i v2v3 = v3 - v2;
	const Vec3i v1v3 = v3 - v1;

	float tCommon = 1.0f / v1v3.x;
	bool isLeft = true;

	for (int x = v1.x; x <= v3.x; ++x) {
		if (x == v2.x) {
			if (v2.x == v3.x) {
				// Выйти, иначе далее будет деление на 0 и бесконечный цикл.
				return;
			}
			// Теперь рисовать правую часть треугольника.
			isLeft = false;
		}
		// Определить точку на векторе v1v3 (общей стороне треугольника), которая соответствует текущему x.
		Vec3i vCommon = v1 + ((x - v1.x) * tCommon) * v1v3;

		// Аналогично определить точку на левой (v1v2) или правой (v2v3) малой стороне треугольника.
		Vec3i v;
		if (isLeft) {
			const float t = 1.0f / v1v2.x;
			v = v1 + ((x - v1.x) * t) * v1v2;
		}
		else {
			const float t = 1.0f / v2v3.x;
			v = v2 + ((x - v2.x) * t) * v2v3;
		}

		// Закрасить вертикальный отрезок от vLow.y до vHigh.y.
		const Vec3i& vLow = (vCommon.y < v.y) ? vCommon : v;
		const Vec3i& vHigh = (vCommon.y < v.y) ? v : vCommon;
		const int yDiff = vHigh.y - vLow.y;
		const int zDiff = vHigh.z - vLow.z;
		int pixelIndex = vLow.y * image.get_width() + x; // pixelIndex >= 0

		for (int y = vLow.y; y <= vHigh.y; ++y) {
			const float zt = static_cast<float>(y - vLow.y) / yDiff;
			const int z = vLow.z + static_cast<int>(std::round(zt * zDiff));

			// Проверить по zBuffer, можно ли рисовать пиксель.
			if (z > zBuffer[pixelIndex]) {
				zBuffer[pixelIndex] = z;
				image.set(x, y, color);
			}

			pixelIndex += image.get_width();
		}
	}
//	for (int x = v1.x; x <= v3.x; ++x) {
//		if (x == v2.x) {
//			if (v2.x == v3.x) {
//				// Выйти, иначе будет деление на 0 при вычислении errDiff и позднее - бесконечный цикл.
//				return;
//			}
//			// Задать значения для рисования правой части треугольника.
//			yIncrease = v3.y > v2.y;
//			errDiff = 1.0f / (v3.x - v2.x) * std::abs(v3.y - v2.y);
//			err = 0;
//			y = v2.y;
//		}
//
//		// Закрасить вертикальный отрезок от yMin до yMax.
//		int yMin = std::min(yCommon, y);
//		int yMax = std::max(yCommon, y);
//		for (int yy = yMin; yy <= yMax; ++yy) {
//			const std::size_t pixelIndex = static_cast<std::size_t>(yy * image.get_width() + x);
//
//			image.set(x, yy, color);
//		}
//
//		// Вычислить y общей стороны треугольника (v1_v3).
//		errCommon += errCommonDiff;
//		while (errCommon >= 0.5f) {
//			errCommon -= 1.0f;
//			yCommon += yCommonIncrease ? 1 : -1;
//		}
//
//		// Вычислить y левой (v1_v2) или правой (v2_v3) короткой стороны треугольника.
//		err += errDiff;
//		while (err >= 0.5f) {
//			err -= 1.0f;
//			y += yIncrease ? 1 : -1;
//		}
//	}
}