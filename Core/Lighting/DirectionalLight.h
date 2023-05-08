#pragma once
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Bindable/ConstantBuffers.h"
#include <DirectXMath.h>

namespace Kaka
{
	class DirectionalLight
	{
	public:
		DirectionalLight(const Graphics& aGfx, const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx);
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
		void Reset();
	private:
		struct DirectionalLightBuffer
		{
			DirectX::XMFLOAT3 lightDirection;
			float padding;
			DirectX::XMFLOAT3 lightColour;
			float ambientLight;
		};

	private:
		DirectionalLightBuffer bufferData;
		PixelConstantBuffer<DirectionalLightBuffer> cbuf;
	};
}
