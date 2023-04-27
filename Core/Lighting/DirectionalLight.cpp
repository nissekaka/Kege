#include "DirectionalLight.h"
#include <External/include/imgui/imgui.h>
#include <d3d11.h>
#include <wrl/client.h>


namespace Kaka
{
	DirectionalLight::DirectionalLight()
	{
		Reset();
	}

	void DirectionalLight::Bind(const Graphics& aGfx)
	{
		// Create constant buffer for directional light

		Microsoft::WRL::ComPtr<ID3D11Buffer> pDirectionalLightBuffer;
		D3D11_BUFFER_DESC dbd = {};
		dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		dbd.Usage = D3D11_USAGE_DYNAMIC;
		dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		dbd.MiscFlags = 0u;
		dbd.ByteWidth = sizeof(bufferData);
		dbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA dsd = {};
		dsd.pSysMem = &bufferData;
		aGfx.pDevice->CreateBuffer(&dbd, &dsd, &pDirectionalLightBuffer);

		// Bind directional light buffer to vertex shader
		aGfx.pContext->VSSetConstantBuffers(1u, 1u, pDirectionalLightBuffer.GetAddressOf());
		aGfx.pContext->PSSetConstantBuffers(1u, 1u, pDirectionalLightBuffer.GetAddressOf());
	}

	void DirectionalLight::ShowControlWindow(const char* aWindowName)
	{
		// Window name defaults to "Light"
		aWindowName = aWindowName ? aWindowName : "Light";

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
		bufferData =
		{
			{-1.0f, -1.0f, 1.0f},
			0.0f,
			{1.0f, 1.0f, 1.0f},
			0.4f
		};
	}
}
