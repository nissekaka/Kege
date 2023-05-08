#pragma once
#include "Core/Graphics/Graphics.h"
#include "Core/Model/Model.h"
#include <DirectXMath.h>

namespace Kaka
{
	class PointLight
	{
	public:
		PointLight(const Graphics& aGfx, const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx, DirectX::FXMMATRIX aView);
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
		void SetModelPosition(Model& aModel);
		void Reset();
	private:
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
		};

	private:
		PointLightBuffer bufferData;
		PixelConstantBuffer<PointLightBuffer> cbuf;
	};
}
