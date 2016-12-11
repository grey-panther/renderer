#include <math.h>
#include "src/tgaimage.h"
#include "src/draw_tools.h"
#include "src/3d_model.cpp"
#include "src/line_model.cpp"

const TGAColor WHITE_COLOR = TGAColor(255, 255, 255, 255);
const TGAColor RED_COLOR   = TGAColor(255, 0,   0,   255);

//const int IMAGE_SIZE = 1800;
const int IMAGE_SIZE = 200;

void drawModelEdges(TGAImage &image, ObjFormatModel model);
void sortByX(array<Point, 3> &points);

void drawTriangle(Point p1, Point p2, Point p3, TGAImage &image, TGAColor color);

int main() {
    ObjFormatModel model("../assets/african_head.obj");
    TGAImage image(IMAGE_SIZE, IMAGE_SIZE, TGAImage::RGB);

    Point p1(180, 50, 0);
    Point p2(150, 1, 0);
//    Point p1(110, 50, 0);
//    Point p2(90, 1, 0);
    Point p3(70, 180, 0);

    drawTriangle(p1, p2, p3, image, WHITE_COLOR);

//    drawModelEdges(image, model);

    image.flip_vertically(); // origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

    return 0;
}

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
//void drawTriangle(Point p1, Point p2, Point p3, TGAImage &image, TGAColor color)
//{
//    drawLine(p1.x, p1.y, p2.x, p2.y, image, color);
//    drawLine(p2.x, p2.y, p3.x, p3.y, image, color);
//    drawLine(p1.x, p1.y, p3.x, p3.y, image, color);
//
//    // Отсортировать точки по x координате
//    const int pointsCount = 3;
//    array<Point, pointsCount> points = {p1, p2, p3};
//
//    for (int i = 0; i < pointsCount - 1; i++) {
//        int minPointIndex = i;
//        for (int j = i + 1; j < pointsCount; j++) {
//            if (points[j].x < points[i].x) {
//                minPointIndex = j;
//            }
//
//            Point buffPoint = points[i];
//            points[i] = points[minPointIndex];
//            points[minPointIndex] = buffPoint;
//        }
//    }
//
//    // Получить уравнения 3х прямых
//    Line2DModel commonLine(points[0].x, points[0].y, points[2].x, points[2].y);
//    Line2DModel leftLine(points[0].x, points[0].y, points[1].x, points[1].y);
//    Line2DModel rightLine(points[1].x, points[1].y, points[2].x, points[2].y);
//
//    // Проходом слева-направо нарисовать вертикальные отрезки, ограниченные 3-мя найденными прямыми
//    for (int x = (int) round(points[0].x); x < (int) round(points[2].x); x++) {
//        Line2DModel currentPartLine = (x < (int) round(points[1].x)) ? leftLine : rightLine;
//
//        if (isnan(currentPartLine.calculateY(x)) || isnan(commonLine.calculateY(x))) {
//            continue;
//        }
//
//        drawLine(x, (int) round(currentPartLine.calculateY(x)), x, (int) round(commonLine.calculateY(x)), image, color);
//    }
//}

void drawModelEdges(TGAImage &image, ObjFormatModel model) {
    int halfImageSize = image.get_width() / 2;
    vector<Point> vertexes = model.getVertexes();
    vector<array<int, ObjFormatModel::FACE_VERTEXES_COUNT>> faces = model.getFaces();

    int vertexesCount = (int) vertexes.size();
    int facesCount = (int) faces.size();
    cout << "vertexes count = " << vertexesCount << endl;
    cout << "faces count = " << facesCount << endl;

    // Нарисовать все рёбра модели (каждое ребро, разделяемое 2мя гранями, рисуется дважды)
    for (int faceIndex = 0; faceIndex < facesCount; faceIndex++) {
        array<int, ObjFormatModel::FACE_VERTEXES_COUNT> currentFace = faces[faceIndex];

        for (int faceVertexIndex = 0; faceVertexIndex < ObjFormatModel::FACE_VERTEXES_COUNT; faceVertexIndex++) {
            int currentVertexGlobalIndex = currentFace[faceVertexIndex];
            Point currentVertex = vertexes[currentVertexGlobalIndex];

            int nextFaceVertexIndex = faceVertexIndex + 1;
            if (nextFaceVertexIndex >= ObjFormatModel::FACE_VERTEXES_COUNT) {
                // Последняя вершина имеет ребро с первой вершиной
                nextFaceVertexIndex = 0;
            }
            int nextVertexGlobalIndex = currentFace[nextFaceVertexIndex];
            Point nextVertex = vertexes[nextVertexGlobalIndex];

            int x0 = (int) round(currentVertex.x * halfImageSize) + halfImageSize;
            int y0 = (int) round(currentVertex.y * halfImageSize) + halfImageSize;
            int x1 = (int) round(nextVertex.x * halfImageSize) + halfImageSize;
            int y1 = (int) round(nextVertex.y * halfImageSize) + halfImageSize;
            drawLine(x0, y0, x1, y1, image, WHITE_COLOR);
        }
    }
}

bool valueInRange(int value, int leftBorder, int rightBorder) {
    return  leftBorder <= value < rightBorder;
}

void drawTestLines(TGAImage image) {
    drawLine(100, 100, 200, 100, image, RED_COLOR);
    drawLine(100, 100, 150, 50, image, RED_COLOR);
    drawLine(100, 100, 100, 0, image, RED_COLOR);
    drawLine(100, 100, 50, 50, image, RED_COLOR);
    drawLine(100, 100, 0, 100, image, RED_COLOR);
    drawLine(100, 100, 50, 150, image, RED_COLOR);
    drawLine(100, 100, 100, 200, image, RED_COLOR);
    drawLine(100, 100, 150, 150, image, RED_COLOR);
}

