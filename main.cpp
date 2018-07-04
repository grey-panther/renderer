#include "DrawTools.hpp"
#include "ObjFormatModel.hpp"
#include "Vec3i.hpp"

#include <iostream>
#include <limits>


const TGAColor WHITE_COLOR = TGAColor(255, 255, 255, 255);
const TGAColor RED_COLOR = TGAColor(255, 0, 0, 255);
const TGAColor GREEN_COLOR = TGAColor(0, 255, 0, 255);

const int IMAGE_SIZE = 1800;
//const int IMAGE_SIZE = 200;

void drawModelEdges(TGAImage& image, const ObjFormatModel& model);

void drawModelFaces(TGAImage& image, const ObjFormatModel& model);


int main()
{
	ObjFormatModel model("../assets/african_head.obj");
	TGAImage image(IMAGE_SIZE, IMAGE_SIZE, TGAImage::RGB);

//	const Vec3i p1(180, 50);
//	const Vec3i p2(150, 1);
//	const Vec3i p3(70, 180);
//	drawLine(p1.xy(), p2.xy(), image, RED_COLOR);
//	drawLine(p1.xy(), p3.xy(), image, RED_COLOR);
//	drawLine(p2.xy(), p3.xy(), image, RED_COLOR);
//	drawTriangle(p1, p2, p3, {}, image, WHITE_COLOR);
//	drawTriangle(p3, p1, Vec3i(210, 80), {}, image, GREEN_COLOR);

//	Point p1(180, 50, 0);
//	Point p2(150, 1, 0);
//	Point p3(70, 180, 0);
//	drawTriangle(p1, p2, p3, image, WHITE_COLOR);

	drawModelFaces(image, model);

//	drawModelEdges(image, model);

	image.flip_vertically(); // origin at the left bottom corner of the image
	image.write_tga_file("output.tga");

	return 0;
}


void drawModelFaces(TGAImage& image, const ObjFormatModel& model)
{
	const int halfImageSize = image.get_width() / 2;
	const vector<Point>& vertexes = model.getVertexes();
	const vector<ModelFace>& faces = model.getFaces();

	Point lightVector(0, 0, 1);

	// Инициализировать z-buffer
	const size_t pixelsCount = static_cast<size_t>(image.get_width() * image.get_height());
	std::vector<int> zBuffer(pixelsCount, numeric_limits<int>::min());

	for (const ModelFace& face : faces) {
		const array<int, ModelFace::FACE_VERTEXES_COUNT>& coordVertexIndexes = face.getCoordVertexIndexes();

		Point vertex0 = vertexes[coordVertexIndexes[0]];
		Point vertex1 = vertexes[coordVertexIndexes[1]];
		Point vertex2 = vertexes[coordVertexIndexes[2]];

		// Вычислить цвет
		const Point vector0 = Point::deduct(vertex1, vertex0);
		const Point vector1 = Point::deduct(vertex2, vertex0);
		const Point faceNormalVector = Point::crossMultiply(vector0, vector1).normalize();

		const double colorIntensity = Point::dotMultiply(faceNormalVector, lightVector);
		if (colorIntensity < 0) {
			continue;   // Отсечение невидимых граней
		}
		const unsigned char colorPart = static_cast<unsigned char>(255 * colorIntensity);
		const TGAColor color = TGAColor(colorPart, colorPart, colorPart, 255);

		// Преобразовать координаты, чтобы растянуть рендер модели на всё изображение
		vertex0.mult(halfImageSize).add(halfImageSize);
		vertex1.mult(halfImageSize).add(halfImageSize);
		vertex2.mult(halfImageSize).add(halfImageSize);

//		drawTriangle(vertex0, vertex1, vertex2, zBuffer, image, color);
		drawTriangle(Vec3i(vertex0), Vec3i(vertex1), Vec3i(vertex2), zBuffer, image, color);
	}
}


void drawModelEdges(TGAImage& image, const ObjFormatModel& model)
{
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
