#pragma once
#include "Drawable.h"
#include "Core/Graphics/Drawable/ModelData.h"
#include "Core/Graphics/Bindable/BindableCommon.h"
#include <DirectXMath.h>
#include <string>

namespace Kaka
{
	class Graphics;

	class Model : public Drawable
	{
	public:
		enum class eShaderType { Solid, Light, Phong, AnimPhong };

	public:
		Model() = default;
		Model(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		~Model() override = default;
		void LoadModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		void Draw(const Graphics& aGfx);
		void Update(float aDeltaTime);
		void Animate();
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		void SetRotation(DirectX::XMFLOAT3 aRotation);
		void SetScale(float aScale);
		void SetColour(DirectX::XMFLOAT4 aColour);
		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMMATRIX GetTransform() const override;
		float GetScale() const;
		bool IsLoaded() const;
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
	private:
		Sampler sampler = {};
		VertexBuffer vertexBuffer = {};
		IndexBuffer indexBuffer = {};
		PixelShader pixelShader;
		VertexShader vertexShader;
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
		InputLayout inputLayout;
		Topology topology = {};
		Rasterizer rasterizer = {};
		DepthStencil depthStencil = {};
	private:
		eShaderType shaderType;
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

		TransformParameters transform;

		DirectX::XMFLOAT4 solidColour;
		float specularIntensity = 0.1f;
		float specularPower = 2.0f;

	private:
		ModelData modelData;
		Texture texture;
		std::vector<std::unique_ptr<Bindable>> bindablePtrs;
		bool isLoaded = false;
		int selectedAnimationIndex = -1;
		float animationTime = 0.0f;
		float animationSpeed = 1.0f;
		bool isAnimationPlaying = false;
	};
}
