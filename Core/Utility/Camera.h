#pragma once
#include "Core/Graphics/Graphics.h"

namespace Kaka
{
	class Camera
	{
	public:
		Camera() noexcept;
		DirectX::XMMATRIX GetMatrix() const noexcept;
		void SpawnControlWindow() noexcept;
		void Reset() noexcept;
		void Rotate(float aDx, float aDy) noexcept;
		void Translate(DirectX::XMFLOAT3 aTranslation) noexcept;
		DirectX::XMFLOAT3 GetPos() const noexcept;

	private:
		DirectX::XMFLOAT3 pos;
		float pitch;
		float yaw;
		static constexpr float TRAVEL_SPEED = 12.0f;
		static constexpr float ROTATION_SPEED = 0.0008f;
	};
}
