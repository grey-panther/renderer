#include "DrawTools.hpp"
#include "ObjFormatModel.hpp"
#include "Mat4.hpp"
#include "MathTests.hpp"
#include "Vec3i.hpp"
#include "Vec4.hpp"

#include <iostream>
#include <limits>


static const TGAColor WHITE_COLOR = TGAColor(255, 255, 255, 255);
static const TGAColor RED_COLOR = TGAColor(255, 0, 0, 255);
static const TGAColor GREEN_COLOR = TGAColor(0, 255, 0, 255);
static const TGAColor BLUE_COLOR = TGAColor(0, 0, 255, 255);
static const float PI = 3.1415926f;

const int IMAGE_SIZE = 1800;
//const int IMAGE_SIZE = 200;

void drawModelEdges(TGAImage& image, const ObjFormatModel& model);

void drawModelFaces(TGAImage& image, const ObjFormatModel& model, const TGAImage& texture, const Mat4& transformMatrix);

Mat4 getViewMatrix();

Mat4 getModelTransformMatrix(int renderTargetWidth, int renderTargetHeight);

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

	const Mat4 viewMatrix = getViewMatrix();
	const Mat4 modelMatrix = getModelTransformMatrix(image.get_width(), image.get_height());
	const Mat4 resultMatrix = viewMatrix * modelMatrix;

	drawModelFaces(image, model, texture, resultMatrix);

//	drawModelEdges(image, model);

	// Draw basis vectors
	Vec4 center {0, 0, 0, 1};
	Vec4 i {100, 0, 0, 1};
	Vec4 j {0, 100, 0, 1};
	Vec4 k {0, 0, 100, 1};
	center = viewMatrix * center;
	i = viewMatrix * i;
	j = viewMatrix * j;
	k = viewMatrix * k;
	drawLine(Vec2i(center.x, center.y), Vec2i(i.x, i.y), image, RED_COLOR);
	drawLine(Vec2i(center.x, center.y), Vec2i(j.x, j.y), image, GREEN_COLOR);
	drawLine(Vec2i(center.x, center.y), Vec2i(k.x, k.y), image, BLUE_COLOR);

	image.flip_vertically(); // origin at the left bottom corner of the image
	image.write_tga_file("output.tga");

	return 0;
}


Mat4 getViewMatrix()
{
	const float angleY = - PI / 4;
	using std::cos;
	using std::sin;
	const Mat4 viewRotationY = {
			cos(angleY),	0,	sin(angleY),	0,
			0, 				1,	0,				0,
			-sin(angleY),	0,	cos(angleY),	0,
			0, 				0,	0,				1,
	};
	const float angleX = PI / 8;
	const Mat4 viewRotationX = {
			1,	0,				0,				0,
			0,	cos(angleX),	-sin(angleX),	0,
			0,	sin(angleX),	cos(angleX),	0,
			0,	0,				0,				1,
	};
	const Mat4 viewTranslation = {
			1,	0,	0,	100,
			0, 	1,	0,	200,
			0,	0,	1,	1000,
			0, 	0,	0,	1,
	};
	const Mat4 viewMatrix = viewTranslation * viewRotationX * viewRotationY;

	return viewMatrix;
}


Mat4 getModelTransformMatrix(int renderTargetWidth, int renderTargetHeight)
{
	const float angle = PI / 2;
	using std::cos;
	using std::sin;
	const Mat4 rotationMatrixX {
			1.f,	0.f,		0.f,			0.f,
			0.f,	cos(angle),	-sin(angle),	0.f,
			0.f,	sin(angle), cos(angle), 	0.f,
			0.f,	0.f,		0.f,			1.f,
	};
	const Mat4 rotationMatrixY {
			cos(angle),		0.f,	sin(angle),	0.f,
			0.f,			1.f,	0.f,		0.f,
			-sin(angle),	0.f,	cos(angle),	0.f,
			0.f,			0.f,	0.f,		1.f,
	};
	const Mat4 rotationMatrixZ {
			cos(angle),	-sin(angle),	0.f,	0.f,
			sin(angle),	cos(angle),		0.f,	0.f,
			0.f,		0.f,			1.f,	0.f,
			0.f,		0.f,			0.f,	1.f,
	};

	const float scale = renderTargetHeight / 3.f;
	const Mat4 scaleMatrix {
			scale,	0.f,	0.f,	0.f,
			0.f,	scale,	0.f,	0.f,
			0.f,	0.f,	scale,	0.f,
			0.f,	0.f,	0.f,	1.f,
	};

	// Позиция в мировых координатах - центр image.
	const Vec3f t { renderTargetWidth * 0.5f, renderTargetHeight * 0.5f, 0.f};
	const Mat4 translateMatrix {
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1,
	};

	// Next transformations are applied in order from right to left:
	// rotation - first
	// scale - second
	// translation - third
//	const Mat4 transformMatrix = translateMatrix * scaleMatrix * rotationMatrixY;
	const Mat4 transformMatrix = translateMatrix * scaleMatrix;
	return transformMatrix;
}


Vec4 getCentralProjection(const Vec4& point)
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

	const Vec4 cameraPoint = cameraMatrix * point;
	const auto projectedPoint = Vec4(cameraPoint.xyz() / cameraPoint.w, 1);
	return projectedPoint;
}


void drawModelFaces(TGAImage& image, const ObjFormatModel& model, const TGAImage& texture, const Mat4& transformMatrix)
{
	const std::vector<Vec3f>& vertexes = model.getVertexes();
	const std::vector<ModelFace>& faces = model.getFaces();

	const Vec3f lightVector(0, 0, 1);

	// Инициализировать z-buffer
	const size_t pixelsCount = static_cast<size_t>(image.get_width() * image.get_height());
	std::vector<int> zBuffer(pixelsCount, std::numeric_limits<int>::min());

	for (const ModelFace& face : faces) {
		const ModelFace::Indices& coordVertexIndexes = face.getCoordVertexIndexes();

		// Нормализованные (от 0 до 1) координаты вершин треугольника.
		auto vertex0 = Vec4(vertexes[coordVertexIndexes[0]], 1);
		auto vertex1 = Vec4(vertexes[coordVertexIndexes[1]], 1);
		auto vertex2 = Vec4(vertexes[coordVertexIndexes[2]], 1);

		// Apply transformations.
		vertex0 = transformMatrix * vertex0;
		vertex1 = transformMatrix * vertex1;
		vertex2 = transformMatrix * vertex2;

		// Применить центральную проекцию ко всем вершинам треугольника.
		/*
		vertex0 = getCentralProjection(vertex0);
		vertex1 = getCentralProjection(vertex1);
		vertex2 = getCentralProjection(vertex2);
		*/

		// Получить нормализованные текстурные координаты.
		const std::array<int, ModelFace::FACE_VERTEXES_COUNT> tvIndexes = face.getTextureVertexIndexes();
		const std::vector<Vec3f>& textureVertexes = model.getTextureVertexes();
		const Vec3f& tv0 = textureVertexes[tvIndexes[0]];
		const Vec3f& tv1 = textureVertexes[tvIndexes[1]];
		const Vec3f& tv2 = textureVertexes[tvIndexes[2]];

		// Получить нормали к каждой из вершин.
		const ModelFace::Indices& vnIndexes = face.getVertexNormalIndexes();
		const std::vector<Vec3f>& vertexNormals = model.getVertexNormals();
		const Vec3f& normal0 = vertexNormals[vnIndexes[0]];
		const Vec3f& normal1 = vertexNormals[vnIndexes[1]];
		const Vec3f& normal2 = vertexNormals[vnIndexes[2]];

		// Пока преобразования аффинные (без перспективных искажений),
		// можно преобразовывать нормали с помощью той же матрицы transformMatrix.
		// Иначе надо использовать транспонированную обратную матрицу к transformMatrix.
		const Vec4 transformedNormal0 = transformMatrix * Vec4(normal0, 0);
		const Vec4 transformedNormal1 = transformMatrix * Vec4(normal1, 0);
		const Vec4 transformedNormal2 = transformMatrix * Vec4(normal2, 0);
		const Vec3f n0 = transformedNormal0.xyz().normalize();
		const Vec3f n1 = transformedNormal1.xyz().normalize();
		const Vec3f n2 = transformedNormal2.xyz().normalize();

		drawTriangle(
				{
					VertexData(vertex0.xyz().round(), tv0, n0),
					VertexData(vertex1.xyz().round(), tv1, n1),
					VertexData(vertex2.xyz().round(), tv2, n2),
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
