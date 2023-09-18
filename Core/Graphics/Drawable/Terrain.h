#pragma once
#include "Drawable.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Bindable/BindableCommon.h"

namespace Kaka
{
	struct Vertex;

	struct TerrainSubset
	{
		std::vector<Vertex> vertices = {};
		std::vector<unsigned short> indices = {};
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;
	};

	class Terrain : public Drawable
	{
	public:
		void Init(const Graphics& aGfx, int aSize);
		void Draw(Graphics& aGfx);
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		void Move(DirectX::XMFLOAT3 aDistance);
		void FlipScale(float aHeight, bool aReset);
		void SetReflectShader(const Graphics& aGfx, bool aValue);
		void SetCullingMode(eCullingMode aMode);
		DirectX::XMFLOAT3 GetRandomVertexPosition() const;
		DirectX::XMMATRIX GetTransform() const override;

	public:
		void ShowControlWindow(const char* aWindowName = nullptr);

	private:
		Sampler sampler = {};
		PixelShader pixelShader;
		VertexShader vertexShader;
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
		};
		InputLayout inputLayout;
		Topology topology = {};
		Rasterizer rasterizer = {};
		DepthStencil depthStencil = {};

	private:
		std::vector<TerrainSubset> terrainSubsets = {};

		struct TransformParameters
		{
			float roll = 0.0f;
			float pitch = 0.0f;
			float yaw = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};
		};

		TransformParameters transform;

		struct PSMaterialConstant
		{
			BOOL normalMapEnabled = TRUE;
			BOOL materialEnabled = TRUE;
			float specularIntensity = 0.1f;
			float specularPower = 2.0f;
		};

		PSMaterialConstant pmc;

		Texture texture{2u};
	};
}