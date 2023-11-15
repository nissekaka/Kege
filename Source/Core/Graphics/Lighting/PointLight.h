#pragma once
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Drawable/Model.h"
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
		DirectX::XMFLOAT3 GetPosition() const;
		float GetRadius() const;
		void SetPosition(DirectX::XMFLOAT3 aPosition) const;
		void SetColour(DirectX::XMFLOAT3 aColour) const;
		void SetIntensity(float aIntensity) const;
		void SetRadius(float aRadius) const;
		void SetFalloff(float aFalloff) const;
		void Reset() const;
		void Draw(Graphics& aGfx) const;

	private:
		static constexpr UINT MAX_LIGHTS = 50u; // Needs to be the same in PixelShader

		struct Transforms
		{
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;
		};

		struct PointLightData
		{
			DirectX::XMFLOAT3 position;
			float padding1;
			DirectX::XMFLOAT3 colour;
			float padding2;
			float diffuseIntensity;
			float radius;
			float falloff;
			float padding3;
			BOOL active = false;
			float padding4[3];
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
