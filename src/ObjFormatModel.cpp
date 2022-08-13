#include "ObjFormatModel.hpp"

#include <fstream>
#include <sstream>
#include "Utilities.hpp"


std::vector<std::string> splitString(const std::string& inputString, const char& delimiter, bool skipEmpty);


void ObjFormatModel::initializeFromFile(const std::string& inputFilePath)
{
	std::string inputString;
	std::ifstream file(inputFilePath);

	// All vertices' data (coords, texture coords, normals) must be read before reading faces.
	// Because array size of these data is used to evaluate indices using parseVertexIndex().
	bool faceReadingStarted = false;

	while (getline(file, inputString)) {
		// See the full standard of Wavefront .obj definition here:
		// https://en.wikipedia.org/wiki/Wavefront_.obj_file
		const std::vector<std::string> elements = splitString(inputString, ' ', true);
		const size_t size = elements.size();

		// Empty line.
		if (size <= 0) {
			continue;
		}

		const std::string& label = elements[0];

		// Init geometric vertices (coordinates of vertices in model space).
		// v x y z [w]
		// w is optional and defaults to 1.0.
		if (label == VERTEX_LABEL) {
			assert(size >= 4);
			assert(!faceReadingStarted);
			const std::string& x = elements[1];
			const std::string& y = elements[2];
			const std::string& z = elements[3];
			const std::string w = (size > 4) ? elements[4] : "1.0";
			Vec4 coords(stof(x), stof(y), stof(z), stof(w));
			_coords.push_back(coords);
		}

		// Init texture coordinates.
		// vt u [v [w]]
		// These coordinates vary between 0 and 1.
		// v, w are optional and default to 0.
		if (label == TEXTURE_VERTEX_LABEL) {
			assert(size >= 2);
			assert(!faceReadingStarted);
			const std::string& u = elements[1];
			const std::string v = (size > 2) ? elements[2] : "0";
			const std::string w = (size > 3) ? elements[3] : "0";
			Vec3f coords(stof(u), stof(v), stof(w));
			_textureCoords.push_back(coords);
		}

		// Init vertex normals.
		// vn x y z
		// Normals might not be unit vectors.
		if (label == NORMAL_VERTEX_LABEL) {
			assert(size >= 4);
			assert(!faceReadingStarted);
			const std::string& x = elements[1];
			const std::string& y = elements[2];
			const std::string& z = elements[3];
			const Vec3f normal(stof(x), stof(y), stof(z));
			_normals.push_back(normal);
		}

		// Init polygonal face elements.
		// f v1 v2 v3 ...
		// f v1/vt1 v2/vt2 v3/vt3 ...
		// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
		// f v1//vn1 v2//vn2 v3//vn3 ...
		if (label == FACE_LABEL) {
			assert(size >= 4);
			faceReadingStarted = true;

			// It supports only triangles now (only 3 vertices in a face).
			std::array<int, ModelFace::FACE_VERTEXES_COUNT> coordsIndices = {};
			std::array<int, ModelFace::FACE_VERTEXES_COUNT> textureCoordsIndices = {};
			std::array<int, ModelFace::FACE_VERTEXES_COUNT> normalsIndices = {};

			// Expecting that _coords, _textureCoords and _normals are completely read at this point.
			const auto coordsCount = static_cast<int>(_coords.size());
			const auto textureCoordsCount = static_cast<int>(_textureCoords.size());
			const auto normalsCount = static_cast<int>(_normals.size());

			for (int i = 0; i < ModelFace::FACE_VERTEXES_COUNT; i++) {
				const std::vector<std::string> vertexInfo = splitString(elements[i + 1], '/', false);
				assert(!vertexInfo.empty());	// Expecting at least a coords index.

				if (vertexInfo.size() > 0) {
					coordsIndices[i] = parseVertexIndex(vertexInfo[0], coordsCount);
				}
				else {
					coordsIndices[i] = UNDEFINED_INDEX;
				}

				if (vertexInfo.size() > 1) {
					textureCoordsIndices[i] = parseVertexIndex(vertexInfo[1], textureCoordsCount);
				}
				else {
					textureCoordsIndices[i] = UNDEFINED_INDEX;
				}

				if (vertexInfo.size() > 2) {
					normalsIndices[i] = parseVertexIndex(vertexInfo[2], normalsCount);
				}
				else {
					normalsIndices[i] = UNDEFINED_INDEX;
				}
			}

			const ModelFace face(coordsIndices, textureCoordsIndices, normalsIndices);
			_faces.push_back(face);
		}
	}

	file.close();
}


int ObjFormatModel::parseVertexIndex(const std::string& indexStr, int verticesCount)
{
	assert(verticesCount > 0);
	if (verticesCount <= 0) {
		return UNDEFINED_INDEX;
	}

	if (indexStr.empty()) {
		return UNDEFINED_INDEX;
	}

	int vIndex = stoi(indexStr);

	assert(vIndex != 0);
	if (vIndex == 0) {
		return UNDEFINED_INDEX;
	}

	if (vIndex > 0) {
		// If an index is positive then it refers to the offset in that vertex list, starting at 1.
		vIndex -= 1;
	}
	else if (vIndex < 0) {
		// If an index is negative then it relatively refers to the end of the vertex list.
		// -1 is referring to the last element.
		vIndex = verticesCount + vIndex;
	}

	return vIndex;
}


std::vector<std::string> splitString(const std::string& inputString, const char& delimiter, bool skipEmpty)
{
	std::vector<std::string> elements;
	std::string element;
	std::istringstream iss;
	iss.str(inputString);

	while (getline(iss, element, delimiter)) {
		if (skipEmpty && element.empty()) {
			continue;
		}
		elements.push_back(element);
	}

	return elements;
}
