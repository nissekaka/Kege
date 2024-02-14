#include "stdafx.h"
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

	void Camera::SetRotationDegrees(const float aPitch, const float aYaw)
	{
		pitch = DegToRad(aPitch);
		yaw = DegToRad(aYaw);
	}

	void Camera::SetDirection(const DirectX::XMFLOAT3 aDirection)
	{
		pitch = asin(-aDirection.y);
		yaw = atan2(aDirection.x, aDirection.z);
	}

	DirectX::XMMATRIX Camera::GetView() const
	{
		DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();

		matrix *= DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
		matrix *= DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
		matrix *= DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		return matrix;
	}

	DirectX::XMMATRIX Camera::GetInverseView() const
	{
		return DirectX::XMMatrixInverse(nullptr, GetView());
	}

	DirectX::XMMATRIX Camera::GetProjection() const
	{
		return projection;
	}

	DirectX::XMFLOAT3 Camera::GetPosition() const
	{
		return position;
	}

	DirectX::XMVECTOR Camera::GetForwardVector() const
	{
		return {
			GetInverseView().r[0].m128_f32[2],
			GetInverseView().r[1].m128_f32[2],
			GetInverseView().r[2].m128_f32[2]
		};
	}

	void Camera::SetPerspective(float aWidth, float aHeight, float aVFov, float aNearZ, float aFarZ)
	{
		cameraType = eCameraType::Perspective;

		perspectiveData.width = aWidth;
		perspectiveData.height = aHeight;
		perspectiveData.nearZ = aNearZ;
		perspectiveData.farZ = aFarZ;

		// Convert aVFov to radians if it's specified in degrees
		float vFovRadians = DirectX::XMConvertToRadians(aVFov);

		const float verticalFov = 2 * atan(tan(vFovRadians / 2) * (aHeight / aWidth));

		perspectiveData.vFov = verticalFov;

		projection = DirectX::XMMatrixPerspectiveFovLH(verticalFov, aWidth / aHeight, aNearZ, aFarZ);
	}

	void Camera::SetOrthographic(float aWidth, float aHeight, float aNearZ, float aFarZ)
	{
		cameraType = eCameraType::Orthographic;

		orthographicData.width = aWidth;
		orthographicData.height = aHeight;
		orthographicData.nearZ = aNearZ;
		orthographicData.farZ = aFarZ;

		projection = DirectX::XMMatrixOrthographicLH(aWidth, aHeight, aNearZ, aFarZ);
	}
}
