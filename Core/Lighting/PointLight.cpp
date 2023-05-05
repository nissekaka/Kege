#include "PointLight.h"
#include <External/include/imgui/imgui.h>

namespace Kaka
{
	PointLight::PointLight(const UINT aSlot)
		:
		slot(aSlot)
	{
		Reset();
	}

	void PointLight::Bind(const Graphics& aGfx)
	{
		// Create constant buffer for directional light

		Microsoft::WRL::ComPtr<ID3D11Buffer> pPointLightBuffer;
		D3D11_BUFFER_DESC dbd = {};
		dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		dbd.Usage = D3D11_USAGE_DYNAMIC;
		dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		dbd.MiscFlags = 0u;
		dbd.ByteWidth = sizeof(bufferData);
		dbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA dsd = {};
		dsd.pSysMem = &bufferData;
		aGfx.pDevice->CreateBuffer(&dbd, &dsd, &pPointLightBuffer);

		// Bind directional light buffer to vertex shader
		//aGfx.pContext->VSSetConstantBuffers(slot, 1u, pPointLightBuffer.GetAddressOf());
		aGfx.pContext->PSSetConstantBuffers(slot, 1u, pPointLightBuffer.GetAddressOf());
	}

	void PointLight::ShowControlWindow(const char* aWindowName)
	{
		// Window name defaults to "Light"
		aWindowName = aWindowName ? aWindowName : "Point Light";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &bufferData.position.x, -60.0f, 60.0f, "%.1f");
			ImGui::SliderFloat("Y", &bufferData.position.y, -60.0f, 60.0f, "%.1f");
			ImGui::SliderFloat("Z", &bufferData.position.z, -60.0f, 60.0f, "%.1f");

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
