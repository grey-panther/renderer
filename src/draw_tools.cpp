#include <array>
#include <math.h>
#include "tgaimage.h"
#include "draw_tools.h"

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
        if (errorY >= dx)
        {
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

    int getY() {
        return  _currentY;
    }

    void update() {
        if (_currentX > _maxX) {
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
            if (points[j].x < points[i].x) {
                minPointIndex = j;
            }

            Point buffPoint = points[i];
            points[i] = points[minPointIndex];
            points[minPointIndex] = buffPoint;
        }
    }
}

void drawTriangle(Point p1, Point p2, Point p3, TGAImage &image, TGAColor color) {
    array<Point, 3> points = {p1, p2, p3};
    sortByX(points);

    int x0 = (int) points[0].x;
    int y0 = (int) points[0].y;
    int x1 = (int) points[1].x;
    int y1 = (int) points[1].y;
    int x2 = (int) points[2].x;
    int y2 = (int) points[2].y;

    DrawLineIterator commonLineIterator(x0, y0, x2, y2);
    DrawLineIterator leftLineIterator(x0, y0, x1, y1);
    DrawLineIterator rightLineIterator(x1, y1, x2, y2);

    for (int x = x0; x <= x2; x++) {

        if (x < x1) {
            drawLine(commonLineIterator.getX(), commonLineIterator.getY(), leftLineIterator.getX(), leftLineIterator.getY(), image, color);
            leftLineIterator.update();
        }
        else {
            drawLine(commonLineIterator.getX(), commonLineIterator.getY(), rightLineIterator.getX(), rightLineIterator.getY(), image, color);
            rightLineIterator.update();
        }

        commonLineIterator.update();
    }
}