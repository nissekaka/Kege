#include "Camera.h"
#include "Core/Utility/KakaMath.h"
#include "External/include/imgui/imgui.h"
#include <algorithm>

namespace Kaka
{
	Camera::Camera()
	{
		Reset();
	}

	void Camera::ShowControlWindow()
	{
		if (ImGui::Begin("Camera"))
		{
			ImGui::Text("Position");
			ImGui::DragFloat3("XYZ", &position.x);
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
			ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}

	void Camera::Reset()
	{
		position = {0.0f, 0.0f, -4.0f};
		//position = {0.0f,140.0f,-450.0f};
		pitch = 0.0f;
		yaw = -PI * 2 / 5;
	}

	void Camera::Rotate(const float aDx, const float aDy)
	{
		yaw = WrapAngle(yaw + aDx * ROTATION_SPEED);
		pitch = std::clamp(pitch + aDy * ROTATION_SPEED, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
	}

	void Camera::Translate(DirectX::XMFLOAT3 aTranslation)
	{
		DirectX::XMStoreFloat3(&aTranslation, DirectX::XMVector3Transform(
			                       DirectX::XMLoadFloat3(&aTranslation),
			                       DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
			                       DirectX::XMMatrixScaling(TRAVEL_SPEED, TRAVEL_SPEED, TRAVEL_SPEED)
		                       ));

		position = {
			position.x + aTranslation.x,
			position.y + aTranslation.y,
			position.z + aTranslation.z
		};
	}

	void Camera::SetPosition(const DirectX::XMFLOAT3 aPosition)
	{
		position = aPosition;
	}

	DirectX::XMMATRIX Camera::GetMatrix() const
	{
		using namespace DirectX;

		const DirectX::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		// Apply the camera rotations to a base vector
		const auto lookVector = XMVector3Transform(forwardBaseVector,
		                                           XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
		);
		// Generate camera transform (applied to all objects to arrange them relative
		// to camera position/orientation in world) from cam position and direction
		// camera "top" always faces towards +Y (cannot do a barrel roll)
		const auto camPosition = XMLoadFloat3(&position);
		const auto camTarget = camPosition + lookVector;
		return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}

	DirectX::XMFLOAT3 Camera::GetPosition() const
	{
		return position;
	}
}
