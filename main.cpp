#include "DrawTools.hpp"
#include "ObjFormatModel.hpp"
#include "shaders/MonochromeShader.hpp"
#include "shaders/NormalMapShader.hpp"
#include "shaders/PhongReflectionShader.hpp"
#include "shaders/SimpleLightShader.hpp"
#include "shaders/ToonShader.hpp"
#include "Mat4.hpp"
#include "MathTests.hpp"
#include "Transform.hpp"
#include "Utilities.hpp"
#include "Vec3i.hpp"
#include "Vec4.hpp"

#include <iostream>
#include <memory>
#include <tuple>


static const TGAColor WHITE_COLOR = TGAColor(255, 255, 255, 255);
static const TGAColor RED_COLOR = TGAColor(255, 0, 0, 255);
static const TGAColor GREEN_COLOR = TGAColor(0, 255, 0, 255);
static const TGAColor BLUE_COLOR = TGAColor(0, 0, 255, 255);
static const TGAColor YELLOW_COLOR = TGAColor(255, 255, 0, 255);
static const TGAColor VIOLET_COLOR = TGAColor(255, 0, 255, 255);
static const TGAColor CYAN_COLOR = TGAColor(0, 255, 255, 255);
static const Vec3 WORLD_UP_DIRECTION {0.f, 1.f, 0.f};
static const Vec3 WORLD_CENTER {0.f, 0.f, 0.f};

static const Vec3 LIGHT_WORLD_POS {1.f, 2.f, 1.f};

static const Vec3 CAMERA_WORLD_POS {1.f, 1.f, 2.f};

void drawPlayground(TGAImage& image);

void drawAxes(TGAImage& image, const Mat4& transformMatrix);

void drawModelEdges(TGAImage& image, const ObjFormatModel& model, const Mat4& transformMatrix);

void drawModelFaces(
		const ObjFormatModel& model,
		const IShader& shader,
		TGAImage& outImage,
		ZBuffer& zBuffer
);

Mat4 getViewportMatrix(int width, int height);

Mat4 getAxesViewportMatrix(int width, int height);

Mat4 getProjectionMatrix();

Mat4 getHeadModelTransformMatrix();


int main()
{
	testMat4();
	testTransforms();

	// Load floor model
	const ObjFormatModel floorModel("../assets/floor.obj");
	TGAImage floorTexture(600, 600, TGAImage::RGB);
	floorTexture.read_tga_file("../assets/floor_diffuse.tga");
	floorTexture.flip_vertically();

	// Load cube model
	const ObjFormatModel cubeModel("../assets/cube.obj");
	TGAImage bricksDiffuseMap(1024, 1024, TGAImage::RGB);
	bricksDiffuseMap.read_tga_file("../assets/bricks_diffuse.tga");
	bricksDiffuseMap.flip_vertically();

	// Load head model
	const ObjFormatModel headModel("../assets/african_head.obj");
	TGAImage headTexture(1024, 1024, TGAImage::RGB);
	headTexture.read_tga_file("../assets/african_head_diffuse.tga");
	headTexture.flip_vertically();
	TGAImage headNormalMap(1024, 1024, TGAImage::RGB);
	headNormalMap.read_tga_file("../assets/african_head_nm.tga");
	headNormalMap.flip_vertically();
	TGAImage headSpecularMap(1024, 1024, TGAImage::GRAYSCALE);
	headSpecularMap.read_tga_file("../assets/african_head_spec.tga");
	headSpecularMap.flip_vertically();

	// Inner eyes.
	const ObjFormatModel eyeInnerModel("../assets/african_head_eye_inner.obj");
	TGAImage eyeInnerTexture(256, 256, TGAImage::RGB);
	eyeInnerTexture.read_tga_file("../assets/african_head_eye_inner_diffuse.tga");
	eyeInnerTexture.flip_vertically();

	// Init output image
	const int OUTPUT_IMAGE_WIDTH = 1920;
	const int OUTPUT_IMAGE_HEIGHT = 1080;
	TGAImage outputImage(OUTPUT_IMAGE_WIDTH, OUTPUT_IMAGE_HEIGHT, TGAImage::RGB);

	drawPlayground(outputImage);

	const Mat4 viewportMatrix = getViewportMatrix(outputImage.get_width(), outputImage.get_height());
	const Mat4 projectionMatrix = getProjectionMatrix();
	const Mat4 viewMatrix = calculateViewMatrix(CAMERA_WORLD_POS, WORLD_CENTER, WORLD_UP_DIRECTION);
	const Mat4 viewProjViewportMatrix = viewportMatrix * projectionMatrix * viewMatrix;
	const Vec3 toLightViewSpaceDirection = (calculateNormalsTransform(viewMatrix) * Vec4((LIGHT_WORLD_POS - WORLD_CENTER), 0.f)).xyz().normalize();

	ZBuffer zBuffer = makeZBuffer(outputImage);

	// Draw two crossed planes.
	if (false) {
		SimpleLightShader shader;
		shader.lightVector = toLightViewSpaceDirection;
		shader.texture = TextureSampler(floorTexture);

		const Mat4 plane1Transform = Transform::makeRotationY(PI / 6) * Transform::makeRotationX(PI / 2);
		shader.transform = viewProjViewportMatrix * plane1Transform;
		shader.normalsTransform = calculateNormalsTransform(viewMatrix * plane1Transform);
		drawModelFaces(floorModel, shader, outputImage, zBuffer);

		const Mat4 plane2Transform = Transform::makeRotationY(PI / 6) * Transform::makeRotationZ(PI / 2);
		shader.transform = viewProjViewportMatrix * plane2Transform;
		shader.normalsTransform = calculateNormalsTransform(viewMatrix * plane2Transform);
		drawModelFaces(floorModel, shader, outputImage, zBuffer);
	}

	// Draw horizontal floor.
	{
		const Mat4 floorTransform = Transform::makeTranslation({0.f, -2.f, 0.f}) * Transform::makeScale(2.f);
		SimpleLightShader shader;
		shader.lightVector = toLightViewSpaceDirection;
		shader.texture = TextureSampler(floorTexture);
		shader.transform = viewProjViewportMatrix * floorTransform;
		shader.normalsTransform = calculateNormalsTransform(viewMatrix * floorTransform);
		drawModelFaces(floorModel, shader, outputImage, zBuffer);
	}

	// Draw a cube.
	if (true) {
		const Mat4 cubeLocalTransform = Transform::makeScale(1.0f) * Transform::makeRotationY(0);
		const Mat4 cubeTransform = viewProjViewportMatrix * cubeLocalTransform;
		const Mat4 cubeNormalsTransform = calculateNormalsTransform(viewMatrix * cubeLocalTransform);
		SimpleLightShader shader;
		shader.transform = cubeTransform;
		shader.normalsTransform = cubeNormalsTransform;
		shader.lightVector = toLightViewSpaceDirection;
		shader.texture = TextureSampler(bricksDiffuseMap);
		drawModelFaces(cubeModel, shader, outputImage, zBuffer);

		// Draw normal vector for the right cube face
		if (false) {
			const Vec4 cubeLocalNormal {1, 0, 0, 0};
			const Vec3 cubeViewNormal = (cubeNormalsTransform * cubeLocalNormal).xyz().normalize();
			const Vec4 cubeRightFaceCenter {1.f, 0.f, 0.f, 1.f};
			Vec4 startNormalPos = cubeTransform * cubeRightFaceCenter;
			startNormalPos /= startNormalPos.w;
			const Vec3 endNormalPos = startNormalPos.xyz() + (cubeViewNormal * 100);
			drawLine(
					Vec2(startNormalPos.x, startNormalPos.y).rounded(),
					Vec2(endNormalPos.x, endNormalPos.y).rounded(),
					outputImage,
					VIOLET_COLOR
			);
		}
	}

	// Draw main head model.
	constexpr bool needDrawHead = false;
	const Mat4 headTransform = viewProjViewportMatrix * getHeadModelTransformMatrix();
	const Mat4 headNormalsTransform = calculateNormalsTransform(viewMatrix * getHeadModelTransformMatrix());
	if (needDrawHead) {
		const Vec3& toLightVector = toLightViewSpaceDirection;

		// Initialize a shader.
		constexpr int usedShader = 4;
		std::unique_ptr<IShader> shaderPtr;
		if constexpr (usedShader == 0) {
			auto shader = std::make_unique<SimpleLightShader>();
			shader->transform = headTransform;
			shader->normalsTransform = headNormalsTransform;
			shader->lightVector = toLightVector;
			shader->texture = TextureSampler(headTexture);
			shaderPtr = std::move(shader);
		}
		else if constexpr (usedShader == 1) {
			auto shader = std::make_unique<MonochromeShader>();
			shader->transform = headTransform;
			shader->normalsTransform = headNormalsTransform;
			shader->lightVector = toLightVector;
			shaderPtr = std::move(shader);
		}
		else if constexpr (usedShader == 2) {
			auto shader = std::make_unique<ToonShader>();
			shader->transform = headTransform;
			shader->normalsTransform = headNormalsTransform;
			shader->lightVector = toLightVector;
			shaderPtr = std::move(shader);
		}
		else if constexpr (usedShader == 3) {
			auto shader = std::make_unique<NormalMapShader>();
			shader->transform = headTransform;
			shader->normalsTransform = headNormalsTransform;
			shader->lightVector = toLightVector;
			shader->texture = TextureSampler(headTexture);
			shader->normalMap = TextureSampler(headNormalMap);
			shaderPtr = std::move(shader);
		}
		else {
			auto shader = std::make_unique<PhongReflectionShader>();
			shader->transform = headTransform;
			shader->normalsTransform = headNormalsTransform;
			shader->lightVector = toLightVector;
			shader->lightColor = Vec3(1.f, 1.f, 1.f);
			shader->specularPowerMultiplier = 20.f;
			shader->ambientIntensity = 0.2f;
			shader->diffuseRatio = 0.8f;
			shader->specularRatio = 1.f - shader->diffuseRatio;
			shader->diffuseColorMap = TextureSampler(headTexture);
			shader->normalMap = TextureSampler(headNormalMap);
			shader->specularMap = TextureSampler(headSpecularMap);
			shaderPtr = std::move(shader);
		}

		assertTrueMsg(shaderPtr != nullptr, "Shader must be set before rendering.");
		if (shaderPtr) {
			drawModelFaces(headModel, *shaderPtr, outputImage, zBuffer);
		}

		// Draw wired head.
		if (false) {
			drawModelEdges(outputImage, headModel, headTransform);
		}
	}

	// Draw head eyes
	if (needDrawHead) {
		SimpleLightShader shader;
		shader.lightVector = toLightViewSpaceDirection;
		shader.texture = TextureSampler(eyeInnerTexture);
		shader.transform = headTransform;
		shader.normalsTransform = headNormalsTransform;
		drawModelFaces(eyeInnerModel, shader, outputImage, zBuffer);
	}

	// Draw light vector
	{
		Vec4 startLightPos = viewProjViewportMatrix * Vec4(WORLD_CENTER, 1.f);
		startLightPos /= startLightPos.w;
		const Vec3 endLightPos = startLightPos.xyz() + (toLightViewSpaceDirection * 100);
		drawLine(
				Vec2(startLightPos.x, startLightPos.y).rounded(),
				Vec2(endLightPos.x, endLightPos.y).rounded(),
				outputImage,
				YELLOW_COLOR
		);
	}

	// Draw basic axes.
	const Mat4 axesViewportMatrix = getAxesViewportMatrix(outputImage.get_width(), outputImage.get_height());
	drawAxes(outputImage, axesViewportMatrix * projectionMatrix * viewMatrix);

	outputImage.flip_vertically(); // origin at the left bottom corner of the outputImage
	outputImage.write_tga_file("output.tga");

	return 0;
}


void drawPlayground(TGAImage& outputImage)
{
	if (false) {
		// Exercises from the articles 1-3.
		const Vec3i p1(180, 50, 0);
		const Vec3i p2(150, 1, 0);
		const Vec3i p3(70, 180, 0);
		drawLine(p1.xy(), p2.xy(), outputImage, RED_COLOR);
		drawLine(p1.xy(), p3.xy(), outputImage, RED_COLOR);
		drawLine(p2.xy(), p3.xy(), outputImage, RED_COLOR);
//		drawTriangle(p1, p2, p3, {}, outputImage, WHITE_COLOR);
//		drawTriangle(p3, p1, Vec3i(210, 80), {}, outputImage, GREEN_COLOR);
	}

	if (false) {
		// An exercise from the article 4.в: https://habr.com/ru/articles/249467/

		// Draw a triangle to test that barycentric coordinates may be used to rasterize a triangle.
		// It should be iterated through the bounding rect of the triangle, not through the entire screen rect.
		Vec2i A(10, 10);
		Vec2i B(100, 30);
		Vec2i C(190, 160);
		for (int x = 0; x < outputImage.get_width(); ++x) {
			for (int y = 0; y < outputImage.get_height(); ++y) {
				const auto& [bar, success] = calculateBarycentricCoordinates(A, B, C, Vec2i(x, y));
				if (!success) {
					continue;
				}
				if ((bar.x < 0.f) || (bar.y < 0.f) || (bar.z < 0.f)) {
					// The point (x, y) is out of triangle borders
					continue;
				}
				outputImage.set(x, y, RED_COLOR);
			}
		}
	}
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


Mat4 getAxesViewportMatrix(int width, int height)
{
	// from coordinates in [-1; 1], [-1; 1] to coordinates in [0, width], [0, height]
	// s - size
	// The matrix translates world point (0, 0) to the left bottom corner of the screen.
	const auto s = static_cast<float>(std::min<int>(width, height));
	const auto w = static_cast<float>(width);
	const auto h = static_cast<float>(height);
	const Mat4 matrix {
			s/2,	0,		0,		w/5,
			0,		s/2,	0,		h/5,
			0,		0,		s/2,	0,
			0,		0,		0, 		1,
	};
	return matrix;
}


Mat4 getHeadModelTransformMatrix()
{
	const Mat4 rotationMatrixX = Transform::makeRotationX(0);
	const Mat4 rotationMatrixY = Transform::makeRotationY(0);
	const Mat4 rotationMatrixZ = Transform::makeRotationZ(0);

	const Mat4 scaleMatrix = Transform::makeScale(2.f);

	// Position of model in world space.
	const Vec3 t {0.f, 0.f, 0.f};
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


void drawModelFaces(
		const ObjFormatModel& model,
		const IShader& shader,
		TGAImage& outImage,
		ZBuffer& zBuffer
)
{
	const auto imageSize = outImage.get_width() * outImage.get_height();
	assertTrueMsg(imageSize == zBuffer.size(),
			"image size = " + std::to_string(imageSize) + "\nzBuffer size = " + std::to_string(zBuffer.size()));

	const std::vector<ModelFace>& modelFaces = model.getFaces();
	const std::vector<Vec4>& modelPositions = model.getCoords();
	const std::vector<Vec3>& modelTextureCoordinates = model.getTextureCoords();
	const std::vector<Vec3>& modelNormals = model.getNormals();
	assertFalse(modelNormals.empty());

	for (const ModelFace& face : modelFaces) {
		const ModelFace::Indices& posIndices = face.getCoordsIndices();
		const ModelFace::Indices& texCoordsIndices = face.getTextureCoordsIndices();
		const ModelFace::Indices& normalIndices = face.getNormalsIndices();
		static_assert(ModelFace::FACE_VERTEXES_COUNT == TRIANGLE_VERTICES_COUNT);

		std::array<VertexData, TRIANGLE_VERTICES_COUNT> inVertexArray;
		for (int i = 0; i < inVertexArray.size(); ++i) {
			// Get the normalized position of the vertex in model space.
			inVertexArray[i].position = modelPositions[posIndices[i]];

			// Get the texture normalized coordinates.
			const Vec3& texCoords = modelTextureCoordinates[texCoordsIndices[i]];
			inVertexArray[i].textureCoords = Vec2(texCoords.x, texCoords.y);

			// Get the normal vector of the vertex in model space.
			inVertexArray[i].normal = modelNormals[normalIndices[i]];
		}

		drawTriangle(
				inVertexArray,
				shader,
				zBuffer,
				outImage
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
