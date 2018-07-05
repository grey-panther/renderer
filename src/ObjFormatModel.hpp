#ifndef OBJFORMATMODEL_HPP
#define OBJFORMATMODEL_HPP

#include "Vec3f.hpp"
#include "ModelFace.cpp"

#include <vector>
#include <string>


class ObjFormatModel
{
private:
	static constexpr const char* VERTEX_LABEL = "v";
	static constexpr const char* TEXTURE_VERTEX_LABEL = "vt";
	static constexpr const char* FACE_LABEL = "f";
	static const int VERTEX_DEFINITION_ELEM_COUNT = 4;
	static const int TEXTURE_VERTEX_DEFINITION_ELEM_COUNT = 5;
	static const int FACE_DEFINITION_ELEM_COUNT = 4;

	std::vector<Vec3f> _coordVertexes;
	std::vector<Vec3f> _textureVertexes;
	std::vector<ModelFace> _faces;

public:
	ObjFormatModel(const std::string& inputFilePath) {
		initializeFromFile(inputFilePath);
	}

	~ObjFormatModel() {

	}

	const std::vector<Vec3f>& getVertexes() const {
		return _coordVertexes;
	}

	const std::vector<Vec3f>& getTextureVertexes() const {
		return _textureVertexes;
	}

	const std::vector<ModelFace>& getFaces() const {
		return _faces;
	};

private:
	void initializeFromFile(const std::string& inputFilePath);
};

#endif