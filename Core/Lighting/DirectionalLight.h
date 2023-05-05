#pragma once
#include <DirectXMath.h>

#include "Core/Graphics/Graphics.h"

namespace Kaka
{
	class DirectionalLight
	{
	public:
		DirectionalLight(const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx);
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
		void Reset();
	private:
		//#pragma warning (push)
		//#pragma warning (disable : 4324)
		struct DirectionalLightBuffer
		{
			DirectX::XMFLOAT3 lightDirection;
			float padding;
			DirectX::XMFLOAT3 lightColour;
			float ambientLight;
		};

		//#pragma warning (pop)
	private:
		DirectionalLightBuffer bufferData;
		UINT slot;
	};
}
