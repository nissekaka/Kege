#pragma once
#include "Drawable.h"
#include "Core/Graphics/Drawable/ModelData.h"
#include "Core/Graphics/Bindable/BindableCommon.h"
#include <DirectXMath.h>
#include <string>

#include "Graphics/Animation/AnimationPlayer.h"


namespace Kaka
{
	class Graphics;

	class Model : public Drawable
	{
	public:
		enum class eShaderType { Solid, Light, Phong, AnimPhong, PBR, AnimPBR };

	public:
		Model() = default;
		Model(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		~Model() override = default;
		void Init();
		void LoadModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		void LoadFBXModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		bool LoadFBXAnimation(const std::string& aFilePath);
		void Draw(Graphics& aGfx);
		void DrawFBX(Graphics& aGfx);
		void DrawFBXPtr(Graphics& aGfx);
		void Update(float aDeltaTime);
		void UpdatePtr(float aDeltaTime);
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
		ModelData& GetModelData() { return modelData; }
		AnimatedModelDataPtr& GetAnimatedModelData() { return animatedModelData; }
		DirectX::XMMATRIX& GetBoneTransform(int aBoneIndex);
		DirectX::XMMATRIX& GetBoneTransform(const std::string& aBoneName);
		DirectX::XMMATRIX GetBoneWorldTransform(int aBoneIndex) const;

		void SetPixelShader(const Graphics& aGfx, const std::wstring& aFilePath);
		void SetTexture(Texture* aTexture);

	public:
		void ShowControlWindow(const char* aWindowName = nullptr);

	private:
		Sampler sampler = {};
		VertexBuffer vertexBuffer = {};
		IndexBuffer indexBuffer = {};
		PixelShader* pixelShader = nullptr;
		VertexShader* vertexShader = nullptr;
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
		AnimatedModelDataPtr animatedModelData;
		ModelData modelData;
		//std::vector<std::unique_ptr<Bindable>> bindablePtrs;
		bool isLoaded = false;

		bool drawSkeleton = false;
		bool drawBoneNames = false;

		BOOL nearbyPointLights[50u];
		BOOL nearbySpotLights[50u];

		std::vector<DirectX::XMMATRIX> combinedTransforms;
		std::vector<DirectX::XMMATRIX> finalTransforms;

		AnimationPlayer animationPlayer;
	};
}
