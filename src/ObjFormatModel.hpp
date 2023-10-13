#pragma once

#include "Vec3.hpp"
#include "Vec4.hpp"
#include "ModelFace.h"

#include <vector>
#include <string>


/**
 * @brief Parses and stores Wavefront .obj 3D-model.
 * @details See the full standard of Wavefront .obj definition here: https://en.wikipedia.org/wiki/Wavefront_.obj_file
 */
class ObjFormatModel
{
private:
	static constexpr const char* VERTEX_LABEL = "v";
	static constexpr const char* TEXTURE_VERTEX_LABEL = "vt";
	static constexpr const char* NORMAL_VERTEX_LABEL = "vn";
	static constexpr const char* FACE_LABEL = "f";
	static constexpr int UNDEFINED_INDEX = -1;

	std::vector<Vec4> _coords;			///< Vertex coords in model space.
	std::vector<Vec3> _textureCoords;
	std::vector<Vec3> _normals;			///< Normals to vertices. Might not be unit vectors.
	std::vector<ModelFace> _faces;

public:
	explicit ObjFormatModel(const std::string& inputFilePath)
	{
		initializeFromFile(inputFilePath);
	}

	~ObjFormatModel() = default;

	[[nodiscard]]
	inline const std::vector<Vec4>& getCoords() const
	{
		return _coords;
	}

	[[nodiscard]]
	inline const std::vector<Vec3>& getTextureCoords() const
	{
		return _textureCoords;
	}

	[[nodiscard]]
	inline const std::vector<Vec3>& getNormals() const
	{
		return _normals;
	}

	[[nodiscard]]
	inline const std::vector<ModelFace>& getFaces() const
	{
		return _faces;
	}

private:
	void initializeFromFile(const std::string& inputFilePath);

	// Transforms .obj standard index (counted from 1 or till -1) to C-array index (counted from 0).
	static int parseVertexIndex(const std::string& indexStr, int verticesCount);
};
