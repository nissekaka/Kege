#include "PointLight.h"
#include <External/include/imgui/imgui.h>

namespace Kaka
{
	PointLight::PointLight(const Graphics& aGfx, const UINT aSlot)
		:
		cbuf(aGfx, aSlot)
	{
		Reset();
	}

	void PointLight::Bind(const Graphics& aGfx, DirectX::FXMMATRIX aView)
	{
		PointLightBuffer dataCopy = bufferData;
		const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&bufferData.position);
		XMStoreFloat3(&dataCopy.position, XMVector3Transform(pos, aView));
		cbuf.Update(aGfx, dataCopy);
		cbuf.Bind(aGfx);
	}

	void PointLight::ShowControlWindow(const char* aWindowName)
	{
		aWindowName = aWindowName ? aWindowName : "Point Light";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &bufferData.position.x, -20.0f, 20.0f, "%.1f");
			ImGui::SliderFloat("Y", &bufferData.position.y, -20.0f, 20.0f, "%.1f");
			ImGui::SliderFloat("Z", &bufferData.position.z, -20.0f, 20.0f, "%.1f");

			ImGui::Text("Intensity/Color");
			ImGui::SliderFloat("Intensity", &bufferData.diffuseIntensity, 0.01f, 2.0f, "%.2f");
			ImGui::ColorEdit3("Diffuse Color", &bufferData.diffuseColour.x);

			ImGui::Text("Falloff");
			ImGui::SliderFloat("Constant", &bufferData.attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Linear", &bufferData.attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Quadratic", &bufferData.attQuad, 0.0000001f, 10.0f, "%.7f",
			                   ImGuiSliderFlags_Logarithmic);

			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}

	void PointLight::SetModelPosition(Model& aModel)
	{
		aModel.SetPosition(bufferData.position);
	}

	void PointLight::Reset()
	{
		bufferData.position = {0.0f, 2.0f, 0.0f};
		bufferData.diffuseColour = {1.0f, 1.0f, 1.0f};
		bufferData.diffuseIntensity = 1.0f;
		bufferData.attConst = 1.0f;
		bufferData.attLin = 0.045f;
		bufferData.attQuad = 0.0075f;
	}
}
