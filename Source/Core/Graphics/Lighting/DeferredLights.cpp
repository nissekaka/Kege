#include "stdafx.h"
#include "DeferredLights.h"

#include "imgui/imgui.h"

namespace Kaka
{
	void DeferredLights::Init(Graphics& aGfx)
	{
		lightVS = ShaderFactory::GetVertexShader(aGfx, L"Shaders\\DeferredLight_VS.cso");
		directionalLightPS = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\DeferredDirectionalLight_PS.cso");

		// Initial directional light values
		directionalLightData.lightDirection = {0.6f, -0.6f, -0.8f};
		directionalLightData.lightColour = {1.0f, 0.8f, 0.6f};
		directionalLightData.lightIntensity = 1.0f;
		directionalLightData.ambientLight = 50.0f;


		struct PVertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 tex;
		};

		PVertex _vertices[4] = {
			{
				{-1.0f, -1.0f, 0.0f},
				{0, 1},
			},
			{
				{-1.0f, 1.0f, 0.0f},
				{0, 0},
			},
			{
				{1.0f, -1.0f, 0.0f},
				{1, 1},
			},
			{
				{1.0f, 1.0f, 0.0f},
				{1, 0},
			}
		};

		unsigned short _indices[6] = {0, 1, 2, 2, 1, 3};

		std::vector<PVertex> vertices;
		std::vector<unsigned short> indices;

		for (int i = 0; i < 4; i++)
		{
			vertices.push_back(_vertices[i]);
		}

		for (int i = 0; i < 6; i++)
		{
			indices.push_back(_indices[i]);
		}

		vertexBuffer.Init(aGfx, vertices);
		vertexBuffer.Bind(aGfx);
		indexBuffer.Init(aGfx, indices);
		indexBuffer.Bind(aGfx);

		ied =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
		};
		inputLayout.Init(aGfx, ied, lightVS->GetBytecode());
		topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DeferredLights::Draw(Graphics& aGfx)
	{
		// Directional Light
		PixelConstantBuffer<DirectionalLightData> directionalLightBuffer{aGfx, 1u};
		directionalLightBuffer.Update(aGfx, directionalLightData);
		directionalLightBuffer.Bind(aGfx);

		lightVS->Bind(aGfx);

		vertexBuffer.Bind(aGfx);
		indexBuffer.Bind(aGfx);
		directionalLightPS->Bind(aGfx);
		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);

		aGfx.DrawIndexed(6u);
		// Unbind shader resources
		//ID3D11ShaderResourceView* nullSRVs[1u] = {nullptr};
		//aGfx.pContext->PSSetShaderResources(0u, 1u, nullSRVs);
	}

	void DeferredLights::ShowControlWindow()
	{
		if (ImGui::Begin("Directional Light"))
		{
			ImGui::Text("Direction");
			ImGui::SliderAngle("X", &directionalLightData.lightDirection.x, -180.0f, 180.0f);
			ImGui::SliderAngle("Y", &directionalLightData.lightDirection.y, -180.0f, 180.0f);
			ImGui::SliderAngle("Z", &directionalLightData.lightDirection.z, -180.0f, 180.0f);
			ImGui::Text("Colour");
			ImGui::ColorEdit3("R", &directionalLightData.lightColour.x);
			ImGui::DragFloat("Intensity", &directionalLightData.lightIntensity, 0.0f, 100.0f);
			ImGui::Text("Ambient");
			ImGui::DragFloat("Intensity", &directionalLightData.ambientLight, 0.0f, 100.0f);
		}
		ImGui::End();
	}

	void DeferredLights::SetShadowCamera(const DirectX::XMMATRIX& aCamera)
	{
		directionalLightData.shadowCamera = aCamera;
	}
}
