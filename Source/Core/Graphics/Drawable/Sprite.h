#pragma once
#include "Drawable.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Bindable/BindableCommon.h"

namespace Kaka
{
	struct SpriteVertex;

	class Sprite : public Drawable
	{
	public:
		void Init(const Graphics& aGfx, float aSize, unsigned int aNumberOfSprites);
		void Draw(Graphics& aGfx);
		void SetPosition(DirectX::XMFLOAT3 aPosition, unsigned int aIndex);
		void SetScale(float aScale, unsigned int aIndex);
		float GetRotation(unsigned int aIndex) const { return rotations[aIndex]; }
		void SetRotation(float aRotation, unsigned int aIndex);
		DirectX::XMMATRIX GetTransform() const override;
		DirectX::XMFLOAT3 GetPosition(unsigned int aIndex) const;

	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
		void Update(float aDeltaTime);

	private:
		Sampler sampler = {};
		PixelShader* pixelShader = nullptr;
		VertexShader* vertexShader = nullptr;
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

			// Data from the instance buffer
			{"INSTANCE_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTANCE_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTANCE_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"INSTANCE_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};
		InputLayout inputLayout;

	private:
		std::vector<SpriteVertex> vertices = {};
		std::vector<unsigned short> indices = {};
		//VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* instanceBuffer = nullptr;
		//ID3D11Buffer* spriteRenderBuffer = nullptr;

	private:
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

		struct SpriteRenderBuffer { };

		//TransformParameters transform;
		//std::vector<TransformParameters> transformParams = {};
		std::vector<DirectX::XMMATRIX> transforms = {};
		std::vector<float> rotations = {};
		//float rotation = 0.0f;

		struct PSMaterialConstant
		{
			BOOL normalMapEnabled = TRUE;
			BOOL materialEnabled = TRUE;
			float padding[2] = {};
		};

		PSMaterialConstant pmc;

		Texture* texture = nullptr;
	};
}
