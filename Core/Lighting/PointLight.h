#pragma once
#include "Core/Graphics/Graphics.h"
#include "Core/Model/Model.h"
#include <DirectXMath.h>
#include <array>

namespace Kaka
{
	class PointLight
	{
	public:
		PointLight(const Graphics& aGfx, const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx, DirectX::FXMMATRIX aView);
	public:
		void ShowControlWindow(const char* aWindowName = nullptr) const;
		DirectX::XMMATRIX GetTransform() const;
		void SetPosition(DirectX::XMFLOAT3 aPosition) const;
		void SetColour(DirectX::XMFLOAT3 aColour) const;
		void SetIntensity(float aIntensity) const;
		void Reset() const;
		void Draw(const Graphics& aGfx) const;
	private:
		static constexpr UINT MAX_LIGHTS = 16u; // Needs to be the same in PixelShader

		struct Transforms
		{
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;
		};

		struct PointLightData
		{
			DirectX::XMFLOAT3 position;
			float padding1;
			DirectX::XMFLOAT3 diffuseColour;
			float padding2;
			float diffuseIntensity;
			float attConst;
			float attLin;
			float attQuad;
			BOOL active = false;
			float padding3[3];
		};

		struct PointLightBuffer
		{
			std::array<PointLightData, MAX_LIGHTS> plb = {};
			UINT activeLights;
			float padding[3];
		};

	private:
		PixelConstantBuffer<PointLightBuffer> cBuffer;
		static std::vector<PointLightData> pointLightData;
		static UINT sharedIndex;
		UINT index;
	};

	inline std::vector<PointLight::PointLightData> PointLight::pointLightData = {};
	inline UINT PointLight::sharedIndex = 0;
}