#pragma once
#include "Core/Graphics/Graphics.h"

namespace Kaka
{
	class Camera
	{
	public:
		Camera();
		DirectX::XMMATRIX GetMatrix() const;
		void ShowControlWindow();
		void Reset();
		void Rotate(float aDx, float aDy);
		void Translate(DirectX::XMFLOAT3 aTranslation);
		DirectX::XMFLOAT3 GetPos() const;

	private:
		DirectX::XMFLOAT3 pos;
		float pitch;
		float yaw;
		static constexpr float TRAVEL_SPEED = 12.0f;
		static constexpr float ROTATION_SPEED = 0.0008f;
	};
}