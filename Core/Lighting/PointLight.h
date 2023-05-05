#pragma once
#include <DirectXMath.h>

#include "Core/Graphics/Graphics.h"
#include "Core/Model/Model.h"

namespace Kaka
{
	class PointLight
	{
	public:
		PointLight(const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx);
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
		UINT slot;
	};
}
