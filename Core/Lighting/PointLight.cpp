#include "PointLight.h"
#include <External/include/imgui/imgui.h>
#include <array>

namespace Kaka
{
	PointLight::PointLight(const Graphics& aGfx, const UINT aSlot)
		:
		cbuf(aGfx, aSlot)
	{
		index = sharedIndex;
		assert(sharedIndex < MAX_LIGHTS && "Too many point lights!");
		sharedIndex++;

		Reset();
	}

	void PointLight::Bind(const Graphics& aGfx, DirectX::FXMMATRIX aView)
	{
		PointLightData plData;
		plData.activeLights = sharedIndex;
		for (UINT i = 0; i < plData.activeLights; ++i)
		{
			plData.plb[i] = bufferData[i];
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&bufferData[i].position);
			XMStoreFloat3(&plData.plb[i].position, XMVector3Transform(pos, aView));
		}
		cbuf.Update(aGfx, plData);
		cbuf.Bind(aGfx);
	}

	void PointLight::ShowControlWindow(const char* aWindowName)
	{
		aWindowName = aWindowName ? aWindowName : "Point Light";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &bufferData[index].position.x, -20.0f, 20.0f, "%.1f");
			ImGui::SliderFloat("Y", &bufferData[index].position.y, -20.0f, 20.0f, "%.1f");
			ImGui::SliderFloat("Z", &bufferData[index].position.z, -20.0f, 20.0f, "%.1f");

			ImGui::Text("Intensity/Colour");
			ImGui::SliderFloat("Intensity", &bufferData[index].diffuseIntensity, 0.01f, 2.0f, "%.2f");
			ImGui::ColorEdit3("Diffuse Colour", &bufferData[index].diffuseColour.x);

			ImGui::Text("Falloff");
			ImGui::SliderFloat("Constant", &bufferData[index].attConst, 0.05f, 10.0f, "%.2f",
			                   ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Linear", &bufferData[index].attLin, 0.0001f, 4.0f, "%.4f",
			                   ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Quadratic", &bufferData[index].attQuad, 0.0000001f, 10.0f, "%.7f",
			                   ImGuiSliderFlags_Logarithmic);

			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}

	void PointLight::SetPosition(const DirectX::XMFLOAT3 aPosition)
	{
		bufferData[index].position = aPosition;
	}

	void PointLight::SetColour(const DirectX::XMFLOAT3 aColour)
	{
		bufferData[index].diffuseColour = aColour;
	}

	void PointLight::SetModelPosition(Model& aModel)
	{
		aModel.SetPosition(bufferData[index].position);
	}

	void PointLight::SetModelColour(Model& aModel)
	{
		aModel.SetColour(DirectX::XMFLOAT4(
			bufferData[index].diffuseColour.x,
			bufferData[index].diffuseColour.y,
			bufferData[index].diffuseColour.z,
			1.0f));
	}

	void PointLight::Reset()
	{
		bufferData[index].position = {0.0f, 2.0f, 0.0f};
		bufferData[index].diffuseColour = {1.0f, 1.0f, 1.0f};
		bufferData[index].diffuseIntensity = 1.0f;
		bufferData[index].attConst = 1.0f;
		bufferData[index].attLin = 0.045f;
		bufferData[index].attQuad = 0.0075f;
		bufferData[index].active = true;
	}

	std::array<PointLight::PointLightBuffer, PointLight::MAX_LIGHTS> PointLight::bufferData = {};
	UINT PointLight::sharedIndex = 0;
}
