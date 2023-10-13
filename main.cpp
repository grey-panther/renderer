#include "DrawTools.hpp"
#include "ObjFormatModel.hpp"
#include "Mat4.hpp"
#include "MathTests.hpp"
#include "Transform.hpp"
#include "Vec3i.hpp"
#include "Vec4.hpp"

#include <iostream>
#include <limits>


static const TGAColor WHITE_COLOR = TGAColor(255, 255, 255, 255);
static const TGAColor RED_COLOR = TGAColor(255, 0, 0, 255);
static const TGAColor GREEN_COLOR = TGAColor(0, 255, 0, 255);
static const TGAColor BLUE_COLOR = TGAColor(0, 0, 255, 255);

void drawPlayground(TGAImage& image);

void drawAxes(TGAImage& image, const Mat4& transformMatrix);

void drawModelEdges(TGAImage& image, const ObjFormatModel& model, const Mat4& transformMatrix);

void drawModelFaces(TGAImage& image, const ObjFormatModel& model, const TGAImage& texture, const Mat4& transformMatrix);

Mat4 getViewportMatrix(int width, int height);

Mat4 getProjectionMatrix();

Mat4 getViewMatrix();

Mat4 getModelTransformMatrix();

int main()
{
	testMat4();
	testTransforms();

	// Load floor model
	const ObjFormatModel floorModel("../assets/floor.obj");
	TGAImage floorTexture(600, 600, TGAImage::RGB);
	floorTexture.read_tga_file("../assets/floor_diffuse.tga");
	floorTexture.flip_vertically();

	// Load head model
	const ObjFormatModel headModel("../assets/african_head.obj");
	TGAImage headTexture(1024, 1024, TGAImage::RGB);
	headTexture.read_tga_file("../assets/african_head_diffuse.tga");
	headTexture.flip_vertically();

	// Init output image
	const int OUTPUT_IMAGE_WIDTH = 1920;
	const int OUTPUT_IMAGE_HEIGHT = 1080;
	TGAImage outputImage(OUTPUT_IMAGE_WIDTH, OUTPUT_IMAGE_HEIGHT, TGAImage::RGB);

//	drawPlayground(outputImage);

	const Mat4 viewportMatrix = getViewportMatrix(outputImage.get_width(), outputImage.get_height());
	const Mat4 projectionMatrix = getProjectionMatrix();
	const Mat4 viewMatrix = getViewMatrix();
	const Mat4 modelMatrix = getModelTransformMatrix();
	const Mat4 resultMatrix = viewportMatrix * projectionMatrix * viewMatrix * modelMatrix;

	drawModelFaces(outputImage, floorModel, floorTexture, resultMatrix);

	drawModelFaces(outputImage, headModel, headTexture, resultMatrix);

//	drawModelEdges(outputImage, headModel, resultMatrix);

	// Draw base axes in (0,0,0) in world coordinates
	drawAxes(outputImage, viewportMatrix * projectionMatrix * viewMatrix);

	outputImage.flip_vertically(); // origin at the left bottom corner of the outputImage
	outputImage.write_tga_file("output.tga");

	return 0;
}


// Упражнения из статей 1-3.
void drawPlayground(TGAImage& image)
{
	const Vec3i p1(180, 50, 0);
	const Vec3i p2(150, 1, 0);
	const Vec3i p3(70, 180, 0);
	drawLine(p1.xy(), p2.xy(), image, RED_COLOR);
	drawLine(p1.xy(), p3.xy(), image, RED_COLOR);
	drawLine(p2.xy(), p3.xy(), image, RED_COLOR);
//	drawTriangle(p1, p2, p3, {}, image, WHITE_COLOR);
//	drawTriangle(p3, p1, Vec3i(210, 80), {}, image, GREEN_COLOR);

	Vec3 p4(180, 50, 0);
	Vec3 p5(150, 1, 0);
	Vec3 p6(70, 180, 0);
//	drawTriangle(p4, p5, p6, image, WHITE_COLOR);
}


// Нарисовать оси Декартовой системы координат.
void drawAxes(TGAImage& image, const Mat4& transformMatrix)
{
	Vec4 center {0, 0, 0, 1};
	Vec4 i {1, 0, 0, 1};
	Vec4 j {0, 1, 0, 1};
	Vec4 k {0, 0, 1, 1};
	center = transformMatrix * center;
	i = transformMatrix * i;
	j = transformMatrix * j;
	k = transformMatrix * k;

	// Transform coordinates from homogeneous to 3D-cartesian ones (make w == 1).
	center /= center.w;
	i /= i.w;
	j /= j.w;
	k /= k.w;

	drawLine(Vec2i(center.x, center.y), Vec2i(i.x, i.y), image, RED_COLOR);
	drawLine(Vec2i(center.x, center.y), Vec2i(j.x, j.y), image, GREEN_COLOR);
	drawLine(Vec2i(center.x, center.y), Vec2i(k.x, k.y), image, BLUE_COLOR);
}


// Get matrix that scales an image to the chosen resolution.
Mat4 getViewportMatrix(int width, int height)
{
	// from coordinates in [-1; 1], [-1; 1] to coordinates in [0, width], [0, height]
	// s - size
	const auto s = static_cast<float>(std::min<int>(width, height));
	const auto w = static_cast<float>(width);
	const auto h = static_cast<float>(height);
	const Mat4 matrix {
			s/2,	0,		0,		w/2,
			0,		s/2,	0,		h/2,
			0,		0,		s/2,	0,
			0,		0,		0, 		1,
	};
	return matrix;
}


Mat4 getViewMatrix()
{
	const Mat4 viewRotationX = Transform::makeRotationX(PI / 8); // PI / 8;
	const Mat4 viewRotationY = Transform::makeRotationY(0.f); // -PI / 4;
	const Mat4 viewScale = Transform::makeScale(0.35f);
	const Mat4 viewTranslation = Transform::makeTranslation(Vec3(0.f, -0.5f, -0.3f));

	const Mat4 viewMatrix = viewTranslation * viewScale * viewRotationX * viewRotationY;
	return viewMatrix;
}


Mat4 getModelTransformMatrix()
{
	const Mat4 rotationMatrixX = Transform::makeRotationX(0);
	const Mat4 rotationMatrixY = Transform::makeRotationY(0);
	const Mat4 rotationMatrixZ = Transform::makeRotationZ(0);

	const Mat4 scaleMatrix = Transform::makeScale(2.f);

	// Позиция в мировых координатах - центр image.
	const Vec3 t {0.f, 2.f, 0.f};
	const Mat4 translationMatrix = Transform::makeTranslation(t);

	// Next transformations are applied in order from right to left:
	// rotation - first
	// scale - second
	// translation - third
	const Mat4 transformMatrix = translationMatrix * scaleMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX;
	return transformMatrix;
}


Mat4 getProjectionMatrix()
{
	// Практика из статьи 4.1. "Построение перспективного изображения" (https://habr.com/ru/post/248611/)
	// Применить искажение координат, чтобы создать центральную проекцию.
	// Камера в точке (0, 0, cameraDistance) и смотрит на плоскость z = 0 (плоскость проекции).
	// Чем больше cameraDistance, тем больше изображение похоже на параллельную проекцию.
	static float cameraDistance = 1.5f;
	static Mat4 projectionMatrix{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, -1 / cameraDistance, 1
	};

	return projectionMatrix;
}


void drawModelFaces(TGAImage& image, const ObjFormatModel& model, const TGAImage& texture, const Mat4& transformMatrix)
{
	const std::vector<Vec4>& coords = model.getCoords();
	const std::vector<ModelFace>& faces = model.getFaces();

	const Vec3 lightVector(0, 0, 1);

	// When we use only affine transformations (without perspective distortions),
	// we can transform normals multiplying them by the transformMatrix as we do with vertex positions.
	// It works because the inverse is equal to the transpose, and the transposed inverse of matrix M is M itself.
	// Otherwise, we must multiply transform normals by the transposed inverse of transformMatrix.
	const Mat4& normalsTransform = transformMatrix.getInverse().getTransposed();

	// Инициализировать z-buffer
	const size_t pixelsCount = static_cast<size_t>(image.get_width() * image.get_height());
	std::vector<int> zBuffer(pixelsCount, std::numeric_limits<int>::min());

	for (const ModelFace& face : faces) {
		const ModelFace::Indices& coordVertexIndexes = face.getCoordsIndices();

		// Нормализованные (от 0 до 1) координаты вершин треугольника.
		auto vertex0 = coords[coordVertexIndexes[0]];
		auto vertex1 = coords[coordVertexIndexes[1]];
		auto vertex2 = coords[coordVertexIndexes[2]];

		// Apply transformations.
		vertex0 = transformMatrix * vertex0;
		vertex1 = transformMatrix * vertex1;
		vertex2 = transformMatrix * vertex2;

		// Transform coordinates from homogeneous to 3D-cartesian ones (make w == 1).
		const Vec3 v0 = vertex0.xyz() / vertex0.w;
		const Vec3 v1 = vertex1.xyz() / vertex1.w;
		const Vec3 v2 = vertex2.xyz() / vertex2.w;

		// Получить нормализованные текстурные координаты.
		const std::array<int, ModelFace::FACE_VERTEXES_COUNT> tvIndexes = face.getTextureCoordsIndices();
		const std::vector<Vec3>& textureVertexes = model.getTextureCoords();
		const Vec3& tv0 = textureVertexes[tvIndexes[0]];
		const Vec3& tv1 = textureVertexes[tvIndexes[1]];
		const Vec3& tv2 = textureVertexes[tvIndexes[2]];

		// Получить нормали к каждой из вершин.
		const ModelFace::Indices& vnIndexes = face.getNormalsIndices();
		const std::vector<Vec3>& vertexNormals = model.getNormals();
		// TODO Посчитать нормальные векторы, если vertexNormals пуст. Или как-то по-другому обработать, чтобы не падало
		const Vec3& normal0 = vertexNormals[vnIndexes[0]];
		const Vec3& normal1 = vertexNormals[vnIndexes[1]];
		const Vec3& normal2 = vertexNormals[vnIndexes[2]];

		const Vec4 transformedNormal0 = normalsTransform * Vec4(normal0, 0);
		const Vec4 transformedNormal1 = normalsTransform * Vec4(normal1, 0);
		const Vec4 transformedNormal2 = normalsTransform * Vec4(normal2, 0);
		const Vec3 n0 = transformedNormal0.xyz().normalize();
		const Vec3 n1 = transformedNormal1.xyz().normalize();
		const Vec3 n2 = transformedNormal2.xyz().normalize();

		drawTriangle(
				{
					VertexData(v0.round(), tv0, n0),
					VertexData(v1.round(), tv1, n1),
					VertexData(v2.round(), tv2, n2),
				},
				zBuffer,
				image,
				lightVector,
				texture
		);
	}
}


void drawModelEdges(TGAImage& image, const ObjFormatModel& model, const Mat4& transformMatrix)
{
	const std::vector<Vec4>& vertexes = model.getCoords();
	const std::vector<ModelFace>& faces = model.getFaces();

	int vertexesCount = (int) vertexes.size();
	int facesCount = (int) faces.size();
	std::cout << "vertexes count = " << vertexesCount << std::endl;
	std::cout << "faces count = " << facesCount << std::endl;

	// Нарисовать все рёбра модели (каждое ребро, разделяемое 2мя гранями, рисуется дважды)
	for (int faceIndex = 0; faceIndex < facesCount; faceIndex++) {
		const ModelFace::Indices& coordVertexIndexes = faces[faceIndex].getCoordsIndices();

		for (int i = 0; i < ModelFace::FACE_VERTEXES_COUNT; i++) {
			const int currentVertexGlobalIndex = coordVertexIndexes[i];
			const Vec4 currentVertex = transformMatrix * vertexes[currentVertexGlobalIndex];

			int nextCoordVertexIndex = i + 1;
			if (nextCoordVertexIndex >= ModelFace::FACE_VERTEXES_COUNT) {
				// Последняя вершина имеет ребро с первой вершиной
				nextCoordVertexIndex = 0;
			}
			const int nextVertexGlobalIndex = coordVertexIndexes[nextCoordVertexIndex];
			const Vec4 nextVertex = transformMatrix * vertexes[nextVertexGlobalIndex];

			// From homogeneous to 3D-cartesian.
			const Vec3 from = currentVertex.xyz() / currentVertex.w;
			const Vec3 to = nextVertex.xyz() / nextVertex.w;

			const int x0 = (int) std::round(from.x);
			const int y0 = (int) std::round(from.y);
			const int x1 = (int) std::round(to.x);
			const int y1 = (int) std::round(to.y);
			drawLine(x0, y0, x1, y1, image, WHITE_COLOR);
		}
	}
}
