#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include "Point.cpp"

using namespace std;

class ObjFormatModel {
public:
    static const int FACE_VERTEXES_COUNT = 3;
private:
    static const char SPACE_DELIMITER = ' ';
    static const char SLASH_DELIMITER = '/';
    static constexpr const char* VERTEX_LABEL = "v";
    static constexpr const char* FACE_LABEL = "f";
    static const int VERTEX_DEFINITION_ELEM_COUNT = 4;
    static const int FACE_DEFINITION_ELEM_COUNT = 4;

    vector<Point> _vertexes;
    vector<array<int, FACE_VERTEXES_COUNT>> _faces;

public:
    vector<Point> getVertexes() {
        return _vertexes;
    }

    vector<array<int, FACE_VERTEXES_COUNT>> getFaces() {
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

            // Инициализировать вершины
            if ((size == VERTEX_DEFINITION_ELEM_COUNT) && elements[0] == VERTEX_LABEL) {
                Point vertex(stod(elements[1]), stod(elements[2]), stod(elements[3]));
                _vertexes.push_back(vertex);
            }

            // Инициализировать грани
            if ((size == FACE_DEFINITION_ELEM_COUNT) && elements[0] == FACE_LABEL) {
                array<int, FACE_VERTEXES_COUNT> faceVertexIndexes;
                for (int i = 0; i < FACE_VERTEXES_COUNT; i++) {
                    vector<string> vertexInfo = splitString(elements[i + 1], SLASH_DELIMITER);
                    // Внимание! В стандарте формата описано, что индексы мб отрицательными (-1 - индекс последней вершины)
                    int vertexIndex = stoi(vertexInfo[0]) - 1;
                    faceVertexIndexes[i] = vertexIndex;
                }

                _faces.push_back(faceVertexIndexes);
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