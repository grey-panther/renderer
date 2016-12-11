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
    vector<array<int, ObjFormatModel::FACE_VERTEXES_COUNT>> faces = model.getFaces();

    int vertexesCount = (int) vertexes.size();
    int facesCount = (int) faces.size();

    // Нарисовать все рёбра модели (каждое ребро, разделяемое 2мя гранями, рисуется дважды)
    for (int faceIndex = 0; faceIndex < facesCount; faceIndex++) {
        array<int, ObjFormatModel::FACE_VERTEXES_COUNT> currentFace = faces[faceIndex];

        Point p0 = vertexes[currentFace[0]].mult(halfImageSize).add(halfImageSize);
        Point p1 = vertexes[currentFace[1]].mult(halfImageSize).add(halfImageSize);
        Point p2 = vertexes[currentFace[2]].mult(halfImageSize).add(halfImageSize);
        drawTriangle(p0, p1, p2, image, WHITE_COLOR);
    }
}

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
