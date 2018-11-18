#ifndef OBJFORMATMODEL_HPP
#define OBJFORMATMODEL_HPP

#include "Vec3f.hpp"
#include "ModelFace.h"

#include <vector>
#include <string>


/**
 * @brief Парсит и хранит Wavefront .obj 3D-модель.
 */
class ObjFormatModel
{
private:
	static constexpr const char* VERTEX_LABEL = "v";
	static constexpr const char* TEXTURE_VERTEX_LABEL = "vt";
	static constexpr const char* NORMAL_VERTEX_LABEL = "vn";
	static constexpr const char* FACE_LABEL = "f";
	static const int VERTEX_DEFINITION_ELEM_COUNT = 4;
	static const int TEXTURE_VERTEX_DEFINITION_ELEM_COUNT = 5;
	static const int VERTEX_NORMAL_DEFINITION_ELEM_COUNT = 5; // Почему 5? Потому что после vn идёт 2 пробела почему-то.
	static const int FACE_DEFINITION_ELEM_COUNT = 4;

	std::vector<Vec3f> _coordVertexes;
	std::vector<Vec3f> _textureVertexes;
	std::vector<Vec3f> _vertexNormals;		///< Нормали к вершинам (могут быть не юнит-векторами).
	std::vector<ModelFace> _faces;

public:
	explicit ObjFormatModel(const std::string& inputFilePath) {
		initializeFromFile(inputFilePath);
	}

	~ObjFormatModel() = default;

	const std::vector<Vec3f>& getVertexes() const {
		return _coordVertexes;
	}

	const std::vector<Vec3f>& getTextureVertexes() const {
		return _textureVertexes;
	}

	const std::vector<Vec3f>& getVertexNormals() const {
		return _vertexNormals;
	}

	const std::vector<ModelFace>& getFaces() const {
		return _faces;
	};

private:
	void initializeFromFile(const std::string& inputFilePath);
};

#endif