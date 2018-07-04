#ifndef RENDERER_MODELFACE_H
#define RENDERER_MODELFACE_H

#include <array>

using namespace std;

class ModelFace
{
public:
	static const int FACE_VERTEXES_COUNT = 3;
private:
	array<int, ModelFace::FACE_VERTEXES_COUNT> _coordVertexIndexes;
	array<int, ModelFace::FACE_VERTEXES_COUNT> _textureVertexIndexes;

public:
	ModelFace(array<int, ModelFace::FACE_VERTEXES_COUNT> coordVertexIndexes,
			  array<int, ModelFace::FACE_VERTEXES_COUNT> textureVertexIndexes) {
		_coordVertexIndexes = coordVertexIndexes;
		_textureVertexIndexes = textureVertexIndexes;
	}

	const array<int, ModelFace::FACE_VERTEXES_COUNT>& getCoordVertexIndexes() const {
		return _coordVertexIndexes;
	}

	const array<int, ModelFace::FACE_VERTEXES_COUNT>& getTextureVertexIndexes() const {
		return _textureVertexIndexes;
	};
};

#endif //RENDERER_MODELFACE_H