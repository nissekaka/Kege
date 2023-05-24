#include "DirectionalLight.h"
#include <External/include/imgui/imgui.h>

namespace Kaka
{
	DirectionalLight::DirectionalLight(const Graphics& aGfx, const UINT aSlot)
		:
		cbuf(aGfx, aSlot)
	{
		Reset();
	}

	void DirectionalLight::Bind(const Graphics& aGfx)
	{
		// TODO Fix directional lights!
		//const DirectX::XMVECTOR viewLight = DirectX::XMLoadFloat3(&bufferData.lightDirection);
		//DirectX::XMStoreFloat3(&bufferData.lightDirection, DirectX::XMVector3Transform(viewLight, aGfx.GetCamera()));

		cbuf.Update(aGfx, bufferData);
		cbuf.Bind(aGfx);
	}

	void DirectionalLight::ShowControlWindow(const char* aWindowName)
	{
		aWindowName = aWindowName ? aWindowName : "Directional Light";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Direction");
			ImGui::SliderAngle("X", &bufferData.lightDirection.x, -180.0f, 180.0f);
			ImGui::SliderAngle("Y", &bufferData.lightDirection.y, -180.0f, 180.0f);
			ImGui::SliderAngle("Z", &bufferData.lightDirection.z, -180.0f, 180.0f);
			ImGui::Text("Colour");
			ImGui::ColorEdit3("R", &bufferData.lightColour.x);
			ImGui::Text("Ambient");
			ImGui::SliderFloat("Intensity", &bufferData.ambientLight, 0.0f, 1.0f);
			if (ImGui::Button("Simulate On/Off"))
			{
				if (shouldSimulate)
				{
					DisableSimulation();
				}
				else
				{
					EnableSimulation();
				}
			}
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}

	void DirectionalLight::Reset()
	{
		bufferData.lightDirection = {-1.0f,-1.0f,1.0f};
		bufferData.lightColour = {0.6f,0.6f,0.6f};
		bufferData.ambientLight = 0.1f;
	}

	void DirectionalLight::SetDirection(const DirectX::XMFLOAT3 aDirection)
	{
		bufferData.lightDirection = aDirection;
	}

	void DirectionalLight::SetColour(const DirectX::XMFLOAT3 aColour)
	{
		bufferData.lightColour = aColour;
	}

	void DirectionalLight::EnableSimulation()
	{
		shouldSimulate = true;
	}

	void DirectionalLight::DisableSimulation()
	{
		shouldSimulate = false;
	}

	bool DirectionalLight::ShouldSimulate() const
	{
		return shouldSimulate;
	}
}