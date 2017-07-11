#include <array>
#include <math.h>
#include "tgaimage.h"
#include "draw_tools.hpp"

using namespace std;

// Рисует линию по алгоритму Брезенхэма с int вычислениями
void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
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

    int y = y0;
    int dx = x1 - x0;
    int dx2 = dx * 2;
    int errorY = 0;
    bool increaseY = (y1 >= y0);
    int dErrorY = increaseY ? 2 * (y1 - y0) : 2 * (y0 - y1); // dErrorY > 0

    for (int x = x0; x <= x1; x++) {
        if (coordsSwapped) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }

        errorY += dErrorY;
        if (errorY >= dx) {
            y += increaseY ? 1 : -1;

            errorY -= dx2;
        }
    }
}

// Итератор, выполняющий проход по дискретному отрезку от точки (x0, y0) до точки (x1, y1)
// На каждый update увеличивается X координата и пересчитывается Y
// x0 <= x1
// Если dy < dx, то итератор аналогичен проходу по всем точкам дискретного отрезка в алгоритме Брезенхэма,
// иначе линия по координате y будет прерывистой
class DrawLineIterator {
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
    DrawLineIterator(int x0, int y0, int x1, int y1) {
        // Важно: x0 <= x1
        _maxX = x1;

        _dx = x1 - x0;
        _dx2 = _dx * 2;

        _currentX = x0;
        _currentY = y0;

        _errorY = 0;
        _increaseY = (y1 >= y0);
        _dErrorY = _increaseY ? 2 * (y1 - y0) : 2 * (y0 - y1); // dErrorY > 0
    }

    int getX() {
        return _currentX;
    }

    int getValue() {
        return  _currentY;
    }

    void update() {
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

void sortByX(array<Point, 3> &points) {
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

void drawTriangle(Point p1, Point p2, Point p3, int zBuffer[], TGAImage &image, TGAColor color) {
    array<Point, 3> points = {p1, p2, p3};
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
            drawLineWithDepthMask(x, commonLineIteratorY.getValue(), commonLineIteratorZ.getValue(), x, leftLineIteratorY.getValue(), leftLineIteratorZ.getValue(), zBuffer, image, color);
            leftLineIteratorY.update();
            leftLineIteratorZ.update();
        }
        else {
            drawLineWithDepthMask(x, commonLineIteratorY.getValue(), commonLineIteratorZ.getValue(), x, rightLineIteratorY.getValue(), rightLineIteratorZ.getValue(), zBuffer, image, color);
            rightLineIteratorY.update();
            rightLineIteratorZ.update();
        }

        commonLineIteratorY.update();
        commonLineIteratorZ.update();
    }
}

// Рисует линию по алгоритму Брезенхэма с int вычислениями
// Не рисует точки, чья глубина больше указанной в массиве zBuffer
void drawLineWithDepthMask(int x0, int y0, int z0, int x1, int y1, int z1, int zBuffer[], TGAImage &image, TGAColor color) {
    int imageWidth = image.get_width();

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