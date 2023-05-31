#pragma once
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Drawable/Model.h"
#include <DirectXMath.h>

namespace Kaka
{
	class SpotLight
	{
	public:
		SpotLight(const Graphics& aGfx, UINT aSlot = 0u);
		void Bind(const Graphics& aGfx, DirectX::FXMMATRIX aView);
		void ShowControlWindow(const char* aWindowName = nullptr);
		DirectX::XMMATRIX GetTransform() const;
		void SetDirection(DirectX::XMFLOAT3 aDirection) const;
		void SetInnerAngle(float aAngle) const;
		void SetOuterAngle(float aAngle) const;
		void Reset() const;
		void Draw(const Graphics& aGfx) const;
	private:
		struct SpotLightData
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 direction;
			DirectX::XMFLOAT3 colour;
			float diffuseIntensity;
			float range;
			float falloff;
			float innerAngle;
			float outerAngle;
			BOOL active = false;
		};
	};
}