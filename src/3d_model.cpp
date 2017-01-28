#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include "Point.cpp"
#include "ModelFace.cpp"

using namespace std;

class ObjFormatModel {
private:
    static const char SPACE_DELIMITER = ' ';
    static const char SLASH_DELIMITER = '/';
    static constexpr const char* VERTEX_LABEL = "v";
    static constexpr const char* TEXTURE_VERTEX_LABEL = "vt";
    static constexpr const char* FACE_LABEL = "f";
    static const int VERTEX_DEFINITION_ELEM_COUNT = 4;
    static const int TEXTURE_VERTEX_DEFINITION_ELEM_COUNT = 5;
    static const int FACE_DEFINITION_ELEM_COUNT = 4;

    vector<Point> _coordVertexes;
    vector<Point> _textureVertexes;
    vector<ModelFace> _faces;

public:
    vector<Point> getCoordVertexes() {
        return _coordVertexes;
    }

    vector<Point> getTextureVertexes() {
        return _textureVertexes;
    }

    vector<ModelFace> getFaces() {
        return _faces;
    };

public:
    ObjFormatModel(string inputFilePath) {
        initializeFromFile(inputFilePath);
    }

    ~ObjFormatModel() {

    }

private:
    void initializeFromFile(string inputFilePath) {
        string inputString;
        ifstream file(inputFilePath);

        while(getline(file, inputString)) {
            vector<string> elements = splitString(inputString, SPACE_DELIMITER);
            int size = (int) elements.size();

            // Инициализировать координатные вершины
            if ((size == VERTEX_DEFINITION_ELEM_COUNT) && elements[0] == VERTEX_LABEL) {
                Point vertex(stod(elements[1]), stod(elements[2]), stod(elements[3]));
                _coordVertexes.push_back(vertex);
            }

            // Инициализировать текстурные вершины
            if ((size == TEXTURE_VERTEX_DEFINITION_ELEM_COUNT) && (elements[0] == TEXTURE_VERTEX_LABEL)) {
                Point vertex(stod(elements[2]), stod(elements[3]), 0);
                _textureVertexes.push_back(vertex);
            }

            // Инициализировать грани
            if ((size == FACE_DEFINITION_ELEM_COUNT) && elements[0] == FACE_LABEL) {
                array<int, ModelFace::FACE_VERTEXES_COUNT> coordVertexIndexes;
                array<int, ModelFace::FACE_VERTEXES_COUNT> textureVertexIndexes;

                for (int i = 0; i < ModelFace::FACE_VERTEXES_COUNT; i++) {
                    vector<string> vertexInfo = splitString(elements[i + 1], SLASH_DELIMITER);

                    // Внимание! В стандарте формата описано, что индексы мб отрицательными (-1 - индекс последней вершины)
                    coordVertexIndexes[i] = stoi(vertexInfo[0]) - 1;
                    textureVertexIndexes[i] = stoi(vertexInfo[1]) - 1;
                }

                ModelFace face(coordVertexIndexes, textureVertexIndexes);
                _faces.push_back(face);
            }
        }

        file.close();
    }

    vector<string> splitString(string inputString, char delimiter) {
        vector<string> elements;
        string element;
        istringstream iss;
        iss.str(inputString);

        while(getline(iss, element, delimiter)) {
            elements.push_back(element);
        }

        return elements;
    }
};