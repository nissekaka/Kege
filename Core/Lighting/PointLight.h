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
		void ShowControlWindow(const char* aWindowName = nullptr);
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		void SetColour(DirectX::XMFLOAT3 aColour);
		void SetModelPosition(Model& aModel);
		void SetModelColour(Model& aModel);
		void Reset();
	private:
		static constexpr UINT MAX_LIGHTS = 128u; // Needs to be the same in PixelShader

		struct PointLightBuffer
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

		struct PointLightData
		{
			std::array<PointLightBuffer, MAX_LIGHTS> plb = {};
			UINT activeLights;
			float padding[3];
		};

	private:
		//PointLightBuffer bufferData;
		PixelConstantBuffer<PointLightData> cbuf;
		static std::array<PointLightBuffer, MAX_LIGHTS> bufferData;
		static UINT sharedIndex;
		UINT index;
	};
}
