#pragma once
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Bindable/BindableCommon.h"
#include "Core/Model/Drawable.h"

namespace Kaka
{
	struct Vertex;

	struct TerrainSubset
	{
		TerrainSubset() = default;
		//TerrainSubset(const std::vector<Vertex> aVertices, std::vector<unsigned short> aIndices)
		//	:
		//	vertices(aVertices),
		//	indices(aIndices) {}

		std::vector<Vertex> vertices = {};
		std::vector<unsigned short> indices = {};
	};

	class Terrain : public Drawable
	{
	public:
		void Init(const Graphics& aGfx, int aSize);
		void Draw(const Graphics& aGfx);
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		DirectX::XMMATRIX GetTransform() const override;

	private:
		std::vector<TerrainSubset> terrainSubsets = {};
		//std::vector<Vertex> vertices;
		//std::vector<unsigned short> indices;
		struct TransformParameters
		{
			float roll = 0.0f;
			float pitch = 0.0f;
			float yaw = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			float scale = 1.0f;
		};

		TransformParameters transform;
		Texture texture;
	};
}