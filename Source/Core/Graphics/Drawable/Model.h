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
		enum class eShaderType { Solid, Light, Phong, AnimPhong, PBR };

	public:
		Model() = default;
		Model(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		~Model() override = default;
		void LoadModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		void Draw(Graphics& aGfx);
		void Update(float aDeltaTime);
		void TraverseBoneHierarchy(int aBoneIndex, const DirectX::XMMATRIX& aParentTransform,
		                           std::vector<DirectX::XMFLOAT4X4>& aInterpolatedBoneTransforms,
		                           const Keyframe& aKeyframe1,
		                           const Keyframe& aKeyframe2,
		                           float aT);
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
		void SetNearbyLights(bool aNearbyPointLights[], bool aNearbySpotLights[]);
		void BindPixelShader(const Graphics& aGfx);

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
		eShaderType shaderType = {};

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

		DirectX::XMFLOAT4 solidColour = {};
		float specularIntensity = 0.1f;
		float specularPower = 2.0f;

	private:
		ModelData modelData;
		Texture texture{2u};
		//std::vector<std::unique_ptr<Bindable>> bindablePtrs;
		bool isLoaded = false;
		int selectedAnimationIndex = -1;
		float animationTime = 0.0f;
		float animationSpeed = 1.0f;
		bool isAnimationPlaying = false;

		BOOL nearbyPointLights[50u];
		BOOL nearbySpotLights[50u];
	};
}
