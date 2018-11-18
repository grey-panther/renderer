#ifndef RENDERER_MODELFACE_H
#define RENDERER_MODELFACE_H

#include <array>


class ModelFace
{
public:
	static const int FACE_VERTEXES_COUNT = 3;
	using Indices = std::array<int, ModelFace::FACE_VERTEXES_COUNT>;

private:
	Indices _coordVertexIndexes;
	Indices _textureVertexIndexes;
	Indices _vertexNormalIndexes;

public:
	ModelFace(const Indices& coordVertexIndexes,
			  const Indices& textureVertexIndexes,
			  const Indices& vertexNormalIndexes
	)
			: _coordVertexIndexes(coordVertexIndexes)
			, _textureVertexIndexes(textureVertexIndexes)
			, _vertexNormalIndexes(vertexNormalIndexes)
	{
	}

	const Indices& getCoordVertexIndexes() const {
		return _coordVertexIndexes;
	}

	const Indices& getTextureVertexIndexes() const {
		return _textureVertexIndexes;
	};

	const Indices& getVertexNormalIndexes() const {
		return _vertexNormalIndexes;
	};
};

#endif //RENDERER_MODELFACE_H