#pragma once
#include "Drawable.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Bindable/BindableCommon.h"
#include "Core/Graphics/Lighting/Lighting.h"

namespace Kaka
{
	struct SpriteVertex;

	class Sprite : public Drawable
	{
	public:
		void Init(const Graphics& aGfx, const float aSize, const unsigned aNumberOfSprites, bool aIsVfx, const std::string& aFile);
		void Draw(Graphics& aGfx);
		void SetPosition(DirectX::XMFLOAT3 aPosition, unsigned int aIndex);
		void SetScale(float aScale, unsigned int aIndex);
		//float GetRotation(unsigned int aIndex) const { return rotations[aIndex]; }
		//void SetRotation(float aRotation, unsigned int aIndex);
		DirectX::XMMATRIX GetTransform() const override;
		DirectX::XMFLOAT3 GetPosition(unsigned int aIndex) const;

	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
		bool IsInSpotlightCone(DirectX::XMFLOAT3 aWorldPosition, const SpotlightData& aSpotlightData);
		void Update(const Graphics& aGfx, const float aDeltaTime, const SpotlightData& aSpotlightData);

	private:
		//Sampler sampler = {};
		PixelShader* vfxPixelShader = nullptr;
		PixelShader* deferredPixelShader = nullptr;

		PixelShader* pixelShader = nullptr;
		VertexShader* vertexShader = nullptr;

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
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
		struct SpriteRenderBuffer { };

		std::vector<DirectX::XMMATRIX> transforms = {};
		std::vector<DirectX::XMFLOAT3> startPositions = {};
		std::vector<float> travelRadiuses = {};
		std::vector<float> travelSpeeds = {};
		std::vector<float> travelAngles = {};

		Texture* texture = nullptr;

		unsigned int updateCounter = 0;
		unsigned int updateIndex = 0;
		unsigned int updateIncrease = 0;
	};
}
