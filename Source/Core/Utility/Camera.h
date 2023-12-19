#pragma once
#include "Core/Graphics/Graphics.h"


namespace Kaka
{
	class Camera
	{
	public:
		Camera();
		void ShowControlWindow();
		void Reset();
		void Rotate(float aDx, float aDy);
		void Translate(DirectX::XMFLOAT3 aTranslation);
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		void SetRotationDegrees(float aPitch, float aYaw);
		DirectX::XMMATRIX GetMatrix() const;
		DirectX::XMFLOAT3 GetPosition() const;

	private:
		DirectX::XMFLOAT3 position;
		float pitch;
		float yaw;
		static constexpr float TRAVEL_SPEED = 12.0f;
		static constexpr float ROTATION_SPEED = 0.0008f;
	};
}
