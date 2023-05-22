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
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}

	void DirectionalLight::Reset()
	{
		bufferData.lightDirection = {-1.0f, -1.0f, 1.0f};
		bufferData.lightColour = {0.6f, 0.6f, 0.6f};
		bufferData.ambientLight = 0.4f;
	}
}
