#include <math.h>
#include <iostream>
#include "src/tgaimage.h"
#include "src/draw_tools.h"
#include "src/3d_model.cpp"

const TGAColor WHITE_COLOR = TGAColor(255, 255, 255, 255);
const TGAColor RED_COLOR   = TGAColor(255, 0,   0,   255);

const int IMAGE_SIZE = 1800;
//const int IMAGE_SIZE = 200;

void drawModelEdges(TGAImage &image, ObjFormatModel model);
void drawModelFaces(TGAImage &image, ObjFormatModel model);

int main() {
    ObjFormatModel model("../assets/african_head.obj");
    TGAImage image(IMAGE_SIZE, IMAGE_SIZE, TGAImage::RGB);

//    Point p1(180, 50, 0);
//    Point p2(150, 1, 0);
//    Point p3(70, 180, 0);

//    drawTriangle(p1, p2, p3, image, WHITE_COLOR);
    drawModelFaces(image, model);

//    drawModelEdges(image, model);

    image.flip_vertically(); // origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

    return 0;
}

void drawModelFaces(TGAImage &image, ObjFormatModel model) {
    int halfImageSize = image.get_width() / 2;
    vector<Point> vertexes = model.getVertexes();
    vector<ModelFace> faces = model.getFaces();

    int facesCount = (int) faces.size();

    Point lightVector(0, 0, 1);
    int pixelsCount = image.get_width() * image.get_height();
    int *zBuffer = new int[pixelsCount];
    for (int pixelIndex = 0; pixelIndex < pixelsCount; pixelIndex++) {
        zBuffer[pixelIndex] = INT_MIN;
    }

    for (int faceIndex = 0; faceIndex < facesCount; faceIndex++) {
        array<int, ModelFace::FACE_VERTEXES_COUNT> coordVertexIndexes = faces[faceIndex].getCoordVertexIndexes();

        Point vertex0 = vertexes[coordVertexIndexes[0]];
        Point vertex1 = vertexes[coordVertexIndexes[1]];
        Point vertex2 = vertexes[coordVertexIndexes[2]];

        // Вычислить цвет
        Point vector0 = vertex1.deduct(vertex0);
        Point vector1 = vertex2.deduct(vertex0);
        Point faceNormalVector = Point::crossMultiply(vector0, vector1).normalize();
        double colorIntensity = Point::dotMultiply(faceNormalVector, lightVector);
        if (colorIntensity < 0) {
            continue;   // Отсечение невидимых граней
        }
        unsigned char colorPart = (unsigned char) (255 * colorIntensity);
        TGAColor color = TGAColor(colorPart, colorPart, colorPart, 255);

        // Преобразовать координаты, чтобы растянуть рендер модели на всё изображение
        Point p0 = vertex0.mult(halfImageSize).add(halfImageSize);
        Point p1 = vertex1.mult(halfImageSize).add(halfImageSize);
        Point p2 = vertex2.mult(halfImageSize).add(halfImageSize);

        drawTriangle(p0, p1, p2, zBuffer, image, color);
    }

    delete[](zBuffer);
}

void drawModelEdges(TGAImage &image, ObjFormatModel model) {
    int halfImageSize = image.get_width() / 2;
    vector<Point> vertexes = model.getVertexes();
    vector<ModelFace> faces = model.getFaces();

    int vertexesCount = (int) vertexes.size();
    int facesCount = (int) faces.size();
    cout << "vertexes count = " << vertexesCount << endl;
    cout << "faces count = " << facesCount << endl;

    // Нарисовать все рёбра модели (каждое ребро, разделяемое 2мя гранями, рисуется дважды)
    for (int faceIndex = 0; faceIndex < facesCount; faceIndex++) {
        array<int, ModelFace::FACE_VERTEXES_COUNT> coordVertexIndexes = faces[faceIndex].getCoordVertexIndexes();

        for (int i = 0; i < ModelFace::FACE_VERTEXES_COUNT; i++) {
            int currentVertexGlobalIndex = coordVertexIndexes[i];
            Point currentVertex = vertexes[currentVertexGlobalIndex];

            int nextCoordVertexIndex = i + 1;
            if (nextCoordVertexIndex >= ModelFace::FACE_VERTEXES_COUNT) {
                // Последняя вершина имеет ребро с первой вершиной
                nextCoordVertexIndex = 0;
            }
            int nextVertexGlobalIndex = coordVertexIndexes[nextCoordVertexIndex];
            Point nextVertex = vertexes[nextVertexGlobalIndex];

            int x0 = (int) round(currentVertex.x * halfImageSize) + halfImageSize;
            int y0 = (int) round(currentVertex.y * halfImageSize) + halfImageSize;
            int x1 = (int) round(nextVertex.x * halfImageSize) + halfImageSize;
            int y1 = (int) round(nextVertex.y * halfImageSize) + halfImageSize;
            drawLine(x0, y0, x1, y1, image, WHITE_COLOR);
        }
    }
}
