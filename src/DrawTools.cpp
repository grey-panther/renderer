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


void drawTriangle(
		std::array<VertexData, 3> vertexes,
		std::vector<int>& zBuffer,
		TGAImage& image,
		const Vec3& lightVector,
		const TGAImage& texture
)
{
	// Отсортировать полученные точки в порядке возрастания x.
	std::sort(vertexes.begin(), vertexes.end(), [](const VertexData& v1, const VertexData& v2) {
		return v1.coords.x < v2.coords.x;
	});

	const Vec3i& v1 = vertexes[0].coords;
	const Vec3i& v2 = vertexes[1].coords;
	const Vec3i& v3 = vertexes[2].coords;

	const Vec3i v1v2 = v2 - v1;
	const Vec3i v2v3 = v3 - v2;
	const Vec3i v1v3 = v3 - v1;

	const Vec3& vt1 = vertexes[0].textureCoords;
	const Vec3& vt2 = vertexes[1].textureCoords;
	const Vec3& vt3 = vertexes[2].textureCoords;

	const Vec3 vt1vt2 = vt2 - vt1;
	const Vec3 vt2vt3 = vt3 - vt2;
	const Vec3 vt1vt3 = vt3 - vt1;

	const Vec3& vn1 = vertexes[0].normal;
	const Vec3& vn2 = vertexes[1].normal;
	const Vec3& vn3 = vertexes[2].normal;

	const Vec3 vn1vn2 = vn2 - vn1;
	const Vec3 vn2vn3 = vn3 - vn2;
	const Vec3 vn1vn3 = vn3 - vn1;

	// Чтобы не выполнять деление в цикле, заранее посчитать обратную величину для v1v3.x.
	const float inverseV1V3X = 1.0f / v1v3.x;
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
		const float commonEdgeProgress = (x - v1.x) * inverseV1V3X;
		// Определить точку на векторе v1v3 (общей стороне треугольника), которая соответствует текущему x.
		VertexData drawingVertex1(
				v1 + commonEdgeProgress * v1v3,
				vt1 + commonEdgeProgress * vt1vt3,
				vn1 + commonEdgeProgress * vn1vn3
		);

		// Аналогично определить точку на левой (v1v2) или правой (v2v3) малой стороне треугольника.
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

		// Под номером 1 пусть будет вершина с меньшей Y координатой.
		if (drawingVertex1.coords.y > drawingVertex2.coords.y) {
			std::swap(drawingVertex1, drawingVertex2);
		}
		// Закрасить вертикальный отрезок от vLow.y до vHigh.y.
		const Vec3i& vLow = drawingVertex1.coords;
		const Vec3i& vHigh = drawingVertex2.coords;
		const Vec3& uv1 = drawingVertex1.textureCoords;
		const Vec3& uv2 = drawingVertex2.textureCoords;
		const Vec3& n1 = drawingVertex1.normal;
		const Vec3& n2 = drawingVertex2.normal;
		const int yDiff = vHigh.y - vLow.y;
		const int zDiff = vHigh.z - vLow.z;
		int pixelIndex = vLow.y * image.get_width() + x;

		for (int y = vLow.y; y <= vHigh.y; ++y, pixelIndex += image.get_width()) {

			// Отсечь пиксель, если он не попадает в изображение.
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

			// Проверить по zBuffer, можно ли рисовать пиксель.
			if (z > zBuffer[pixelIndex]) {

				// Определить интенсивность цвета в данной точке треугольника.
				const Vec3 normal = (n1 + t * (n2 - n1)).normalize();
				const float colorIntensity = Vec3::dotMultiply(normal, lightVector);
				if (colorIntensity < 0) {
					// Отсечение невидимой точки.
					// Невидимую точку не записывать в z буффер, а то она может оказаться самой ближней
					// и не дать нарисовать другую точку позади себя.
					continue;
				}

				zBuffer[pixelIndex] = z;

				// Вычислить uv-координаты для данной точки треугольника.
				const Vec3 uv = uv1 + t * (uv2 - uv1);

				// Взять из текстуры цвет по полученным uv координатам.
				const int u = static_cast<int>(std::round(uv.x * texture.get_width()));
				const int v = static_cast<int>(std::round(uv.y * texture.get_height()));
				const TGAColor textureColor = texture.get(u, v);

				// Скорректировать цвета в зависимости от освещённости точки.
				const auto r = static_cast<unsigned char>(textureColor.r * colorIntensity);
				const auto g = static_cast<unsigned char>(textureColor.g * colorIntensity);
				const auto b = static_cast<unsigned char>(textureColor.b * colorIntensity);

				image.set(x, y, TGAColor(r, g, b, 255));
			}
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