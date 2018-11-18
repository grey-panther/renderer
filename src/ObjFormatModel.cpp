#include "ObjFormatModel.hpp"

#include <fstream>
#include <sstream>


std::vector<std::string> splitString(const std::string& inputString, const char& delimiter);

void ObjFormatModel::initializeFromFile(const std::string& inputFilePath) {
	std::string inputString;
	std::ifstream file(inputFilePath);

	while (getline(file, inputString)) {
		std::vector<std::string> elements = splitString(inputString, ' ');
		int size = (int) elements.size();

		// Инициализировать координатные вершины
		if ((size == VERTEX_DEFINITION_ELEM_COUNT) && elements[0] == VERTEX_LABEL) {
			Vec3f vertex(stof(elements[1]), stof(elements[2]), stof(elements[3]));
			_coordVertexes.push_back(vertex);
		}

		// Инициализировать текстурные вершины
		if ((size == TEXTURE_VERTEX_DEFINITION_ELEM_COUNT) && (elements[0] == TEXTURE_VERTEX_LABEL)) {
			Vec3f vertex(stof(elements[2]), stof(elements[3]), 0);
			_textureVertexes.push_back(vertex);
		}

		// Инициализировать нормали к вершинам
		if ((size == VERTEX_NORMAL_DEFINITION_ELEM_COUNT) && (elements[0] == NORMAL_VERTEX_LABEL)) {
			const Vec3f normal(stof(elements[2]), stof(elements[3]), stof(elements[4]));
			_vertexNormals.push_back(normal);
		}

		// Инициализировать грани
		if ((size == FACE_DEFINITION_ELEM_COUNT) && elements[0] == FACE_LABEL) {
			std::array<int, ModelFace::FACE_VERTEXES_COUNT> coordVertexIndexes = {};
			std::array<int, ModelFace::FACE_VERTEXES_COUNT> textureVertexIndexes = {};
			std::array<int, ModelFace::FACE_VERTEXES_COUNT> vertexNormalIndexes = {};

			for (int i = 0; i < ModelFace::FACE_VERTEXES_COUNT; i++) {
				std::vector<std::string> vertexInfo = splitString(elements[i + 1], '/');

				// Внимание! В стандарте формата описано, что индексы мб отрицательными (-1 - индекс последней вершины)
				coordVertexIndexes[i] = stoi(vertexInfo[0]) - 1;
				textureVertexIndexes[i] = stoi(vertexInfo[1]) - 1;
				vertexNormalIndexes[i] = stoi(vertexInfo[2]) - 1;
			}

			const ModelFace face(coordVertexIndexes, textureVertexIndexes, vertexNormalIndexes);
			_faces.push_back(face);
		}
	}

	file.close();
}

std::vector<std::string> splitString(const std::string& inputString, const char& delimiter) {
	std::vector<std::string> elements;
	std::string element;
	std::istringstream iss;
	iss.str(inputString);

	while (getline(iss, element, delimiter)) {
		elements.push_back(element);
	}

	return elements;
}
