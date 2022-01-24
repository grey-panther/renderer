#include "DrawTools.hpp"
#include "ObjFormatModel.hpp"
#include "Mat4.hpp"
#include "MathTests.hpp"
#include "Vec3i.hpp"
#include "Vec4.hpp"

#include <iostream>
#include <limits>


const TGAColor WHITE_COLOR = TGAColor(255, 255, 255, 255);
const TGAColor RED_COLOR = TGAColor(255, 0, 0, 255);
const TGAColor GREEN_COLOR = TGAColor(0, 255, 0, 255);

const int IMAGE_SIZE = 1800;
//const int IMAGE_SIZE = 200;

void drawModelEdges(TGAImage& image, const ObjFormatModel& model);

void drawModelFaces(TGAImage& image, const ObjFormatModel& model, const TGAImage& texture);


int main()
{
	testMat4();

	const ObjFormatModel model("../assets/african_head.obj");
	TGAImage image(IMAGE_SIZE, IMAGE_SIZE, TGAImage::RGB);

	TGAImage texture(1024, 1024, TGAImage::RGB);
	texture.read_tga_file("../assets/african_head_diffuse.tga");
	texture.flip_vertically();

//	const Vec3i p1(180, 50);
//	const Vec3i p2(150, 1);
//	const Vec3i p3(70, 180);
//	drawLine(p1.xy(), p2.xy(), image, RED_COLOR);
//	drawLine(p1.xy(), p3.xy(), image, RED_COLOR);
//	drawLine(p2.xy(), p3.xy(), image, RED_COLOR);
//	drawTriangle(p1, p2, p3, {}, image, WHITE_COLOR);
//	drawTriangle(p3, p1, Vec3i(210, 80), {}, image, GREEN_COLOR);

//	Vec3f p1(180, 50, 0);
//	Vec3f p2(150, 1, 0);
//	Vec3f p3(70, 180, 0);
//	drawTriangle(p1, p2, p3, image, WHITE_COLOR);

	drawModelFaces(image, model, texture);

//	drawModelEdges(image, model);

	image.flip_vertically(); // origin at the left bottom corner of the image
	image.write_tga_file("output.tga");

	return 0;
}


Vec3f getCentralProjection(const Vec3f& point)
{
	// Практика из статьи 4.1. "Построение перспективного изображения" (https://habr.com/ru/post/248611/)
	// Применить искажение координат, чтобы создать центральную проекцию.
	// Камера в точке (0, 0, cameraDistance) и смотрит на плоскость z = 0 (плоскость проекции).
	// Чем больше cameraDistance, тем больше изображение похоже на параллельную проекцию.
	static float cameraDistance = 5.f;
	static Mat4 cameraMatrix{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, -1 / cameraDistance, 1
	};

	const Vec4 cameraPoint = cameraMatrix * Vec4(point, 1);
	const auto projectedPoint = cameraPoint.xyz() / cameraPoint.w;
	return projectedPoint;
}


void drawModelFaces(TGAImage& image, const ObjFormatModel& model, const TGAImage& texture)
{
	const int halfImageSize = image.get_width() / 2;
	const std::vector<Vec3f>& vertexes = model.getVertexes();
	const std::vector<ModelFace>& faces = model.getFaces();

	const Vec3f lightVector(0, 0, 1);

	// Инициализировать z-buffer
	const size_t pixelsCount = static_cast<size_t>(image.get_width() * image.get_height());
	std::vector<int> zBuffer(pixelsCount, std::numeric_limits<int>::min());

	for (const ModelFace& face : faces) {
		const ModelFace::Indices& coordVertexIndexes = face.getCoordVertexIndexes();

		// Нормализованные (от 0 до 1) координаты вершин треугольника.
		Vec3f vertex0 = vertexes[coordVertexIndexes[0]];
		Vec3f vertex1 = vertexes[coordVertexIndexes[1]];
		Vec3f vertex2 = vertexes[coordVertexIndexes[2]];

		// Применить центральную проекцию ко всем вершинам треугольника.
		vertex0 = getCentralProjection(vertex0);
		vertex1 = getCentralProjection(vertex1);
		vertex2 = getCentralProjection(vertex2);

		// Преобразовать координаты, чтобы растянуть рендер модели на всё изображение
		(vertex0 *= halfImageSize) += halfImageSize;
		(vertex1 *= halfImageSize) += halfImageSize;
		(vertex2 *= halfImageSize) += halfImageSize;

		// Получить нормализованные текстурные координаты.
		const std::array<int, ModelFace::FACE_VERTEXES_COUNT> tvIndexes = face.getTextureVertexIndexes();
		const std::vector<Vec3f>& textureVertexes = model.getTextureVertexes();
		const Vec3f& tv0 = textureVertexes[tvIndexes[0]];
		const Vec3f& tv1 = textureVertexes[tvIndexes[1]];
		const Vec3f& tv2 = textureVertexes[tvIndexes[2]];

		// Получить нормали к каждой из вершин.
		const ModelFace::Indices& vnIndexes = face.getVertexNormalIndexes();
		const std::vector<Vec3f>& vertexNormals = model.getVertexNormals();
		const Vec3f& vn0 = vertexNormals[vnIndexes[0]];
		const Vec3f& vn1 = vertexNormals[vnIndexes[1]];
		const Vec3f& vn2 = vertexNormals[vnIndexes[2]];

		drawTriangle(
				{
					VertexData(vertex0.round(), tv0, vn0),
					VertexData(vertex1.round(), tv1, vn1),
					VertexData(vertex2.round(), tv2, vn2),
				},
				zBuffer,
				image,
				lightVector,
				texture
		);
	}
}


void drawModelEdges(TGAImage& image, const ObjFormatModel& model)
{
	int halfImageSize = image.get_width() / 2;
	const std::vector<Vec3f>& vertexes = model.getVertexes();
	const std::vector<ModelFace>& faces = model.getFaces();

	int vertexesCount = (int) vertexes.size();
	int facesCount = (int) faces.size();
	std::cout << "vertexes count = " << vertexesCount << std::endl;
	std::cout << "faces count = " << facesCount << std::endl;

	// Нарисовать все рёбра модели (каждое ребро, разделяемое 2мя гранями, рисуется дважды)
	for (int faceIndex = 0; faceIndex < facesCount; faceIndex++) {
		const ModelFace::Indices& coordVertexIndexes = faces[faceIndex].getCoordVertexIndexes();

		for (int i = 0; i < ModelFace::FACE_VERTEXES_COUNT; i++) {
			int currentVertexGlobalIndex = coordVertexIndexes[i];
			const Vec3f& currentVertex = vertexes[currentVertexGlobalIndex];

			int nextCoordVertexIndex = i + 1;
			if (nextCoordVertexIndex >= ModelFace::FACE_VERTEXES_COUNT) {
				// Последняя вершина имеет ребро с первой вершиной
				nextCoordVertexIndex = 0;
			}
			int nextVertexGlobalIndex = coordVertexIndexes[nextCoordVertexIndex];
			const Vec3f& nextVertex = vertexes[nextVertexGlobalIndex];

			int x0 = (int) round(currentVertex.x * halfImageSize) + halfImageSize;
			int y0 = (int) round(currentVertex.y * halfImageSize) + halfImageSize;
			int x1 = (int) round(nextVertex.x * halfImageSize) + halfImageSize;
			int y1 = (int) round(nextVertex.y * halfImageSize) + halfImageSize;
			drawLine(x0, y0, x1, y1, image, WHITE_COLOR);
		}
	}
}
