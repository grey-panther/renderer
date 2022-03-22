#pragma once

#include <array>


class ModelFace
{
public:
	static const int FACE_VERTEXES_COUNT = 3;
	using Indices = std::array<int, ModelFace::FACE_VERTEXES_COUNT>;

private:
	Indices _coordsIndices;
	Indices _textureCoordsIndices;
	Indices _normalsIndices;

public:
	ModelFace(const Indices& coordsIndices,
			  const Indices& textureCoordsIndices,
			  const Indices& normalsIndices
	)
			: _coordsIndices(coordsIndices)
			, _textureCoordsIndices(textureCoordsIndices)
			, _normalsIndices(normalsIndices)
	{
	}

	[[nodiscard]]
	inline const Indices& getCoordsIndices() const
	{
		return _coordsIndices;
	}

	[[nodiscard]]
	inline const Indices& getTextureCoordsIndices() const
	{
		return _textureCoordsIndices;
	}

	[[nodiscard]]
	inline const Indices& getNormalsIndices() const
	{
		return _normalsIndices;
	}
};
