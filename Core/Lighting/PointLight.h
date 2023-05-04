#pragma once
#include <DirectXMath.h>

#include "Core/Graphics/Graphics.h"
#include "Core/Model/Model.h"

namespace Kaka
{
	class PointLight
	{
	public:
		PointLight(Graphics& aGfx, const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx);
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
		void SetModelPosition(Model& aModel);
		void Reset();
	private:
		struct PointLightBuffer
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 diffuseColour;
			float padding[2];
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