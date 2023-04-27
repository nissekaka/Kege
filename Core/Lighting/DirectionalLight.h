#pragma once
#include <DirectXMath.h>

#include "Core/Graphics/Graphics.h"

namespace Kaka
{
	class DirectionalLight
	{
	public:
		DirectionalLight();
		void Bind(const Graphics& aGfx);
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
		void Reset();
	private:
		struct DirectionalLightBuffer
		{
			DirectX::XMFLOAT3 lightDirection;
			float padding1;
			DirectX::XMFLOAT3 lightColour;
			float ambientLight;
		};

	private:
		DirectionalLightBuffer bufferData;
	};
}
