#include "stdafx.h"
#include "DeferredLights.h"
#include "Core/Utility/KakaMath.h"
#include "imgui/imgui.h"

namespace Kaka
{
	void DeferredLights::CreateSphere(Graphics& aGfx)
	{
		std::vector<PVertex> sphereVertices;
		std::vector<unsigned short> sphereIndices;

		int aLatDiv = 7;
		int aLongDiv = 7;

		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float lattitudeAngle = PI / aLatDiv;
		const float longitudeAngle = 2.0f * PI / aLongDiv;

		for (int iLat = 1; iLat < aLatDiv; iLat++)
		{
			const auto latBase = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationX(lattitudeAngle * iLat)
			);
			for (int iLong = 0; iLong < aLongDiv; iLong++)
			{
				DirectX::XMFLOAT3 calculatedPos;
				auto v = DirectX::XMVector3Transform(
					latBase,
					DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				DirectX::XMStoreFloat3(&calculatedPos, v);
				sphereVertices.emplace_back();
				sphereVertices.back().pos = calculatedPos;
			}
		}

		// Add the cap vertices
		const auto iNorthPole = static_cast<unsigned short>(sphereVertices.size());
		{
			DirectX::XMFLOAT3 northPos;
			DirectX::XMStoreFloat3(&northPos, base);
			sphereVertices.emplace_back();
			sphereVertices.back().pos = northPos;
		}
		const auto iSouthPole = static_cast<unsigned short>(sphereVertices.size());
		{
			DirectX::XMFLOAT3 southPos;
			DirectX::XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base));
			sphereVertices.emplace_back();
			sphereVertices.back().pos = southPos;
		}

		const auto calcIdx = [aLatDiv, aLongDiv](unsigned short aILat, unsigned short aILong)
		{
			return aILat * aLongDiv + aILong;
		};

#pragma warning (push)
#pragma warning (disable : 4244)
		for (unsigned short iLat = 0; iLat < aLatDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < aLongDiv - 1; iLong++)
			{
				sphereIndices.push_back(calcIdx(iLat, iLong));
				sphereIndices.push_back(calcIdx(iLat + 1, iLong));
				sphereIndices.push_back(calcIdx(iLat, iLong + 1));
				sphereIndices.push_back(calcIdx(iLat, iLong + 1));
				sphereIndices.push_back(calcIdx(iLat + 1, iLong));
				sphereIndices.push_back(calcIdx(iLat + 1, iLong + 1));
			}
			// Wrap band
			sphereIndices.push_back(calcIdx(iLat, aLongDiv - 1));
			sphereIndices.push_back(calcIdx(iLat + 1, aLongDiv - 1));
			sphereIndices.push_back(calcIdx(iLat, 0));
			sphereIndices.push_back(calcIdx(iLat, 0));
			sphereIndices.push_back(calcIdx(iLat + 1, aLongDiv - 1));
			sphereIndices.push_back(calcIdx(iLat + 1, 0));
		}

		// Cap fans
		for (unsigned short iLong = 0; iLong < aLongDiv - 1; iLong++)
		{
			// North
			sphereIndices.push_back(iNorthPole);
			sphereIndices.push_back(calcIdx(0, iLong));
			sphereIndices.push_back(calcIdx(0, iLong + 1));
			// South
			sphereIndices.push_back(calcIdx(aLatDiv - 2, iLong + 1));
			sphereIndices.push_back(calcIdx(aLatDiv - 2, iLong));
			sphereIndices.push_back(iSouthPole);
		}
		// Wrap triangles
		// North
		sphereIndices.push_back(iNorthPole);
		sphereIndices.push_back(calcIdx(0, aLongDiv - 1));
		sphereIndices.push_back(calcIdx(0, 0));
		// South
		sphereIndices.push_back(calcIdx(aLatDiv - 2, 0));
		sphereIndices.push_back(calcIdx(aLatDiv - 2, aLongDiv - 1));
		sphereIndices.push_back(iSouthPole);

		sphereVertexBuffer.Init(aGfx, sphereVertices);
		sphereIndexBuffer.Init(aGfx, sphereIndices);
	}

	void DeferredLights::CreateQuad(Graphics& aGfx)
	{
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

		std::vector<PVertex> quadVertices;
		std::vector<unsigned short> quadIndices;

		for (int i = 0; i < 4; i++)
		{
			quadVertices.push_back(_vertices[i]);
		}

		for (int i = 0; i < 6; i++)
		{
			quadIndices.push_back(_indices[i]);
		}

		quadVertexBuffer.Init(aGfx, quadVertices);
		quadIndexBuffer.Init(aGfx, quadIndices);
	}

	void DeferredLights::Init(Graphics& aGfx)
	{
		lightVS = ShaderFactory::GetVertexShader(aGfx, L"Shaders\\DeferredLight_VS.cso");
		directionalLightPS = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\DeferredDirectionalLight_PS.cso");
		pointLightPS = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\DeferredPointLight_PS.cso");
		spotLightPS = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\DeferredSpotLight_PS.cso");

		// Initial directional light values
		directionalLightData.lightDirection = {0.6f, -0.6f, -0.8f};
		directionalLightData.lightColour = {1.0f, 0.8f, 0.6f};
		directionalLightData.lightIntensity = 1.0f;
		directionalLightData.ambientLight = 50.0f;

		CreateQuad(aGfx);

		CreateSphere(aGfx);

		// Create input layout

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
		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);

		// Directional Light
		{
			ConstantBuffer transformBuffer;
			transformBuffer.positionAndRange[0] = 0.0f;
			transformBuffer.positionAndRange[1] = 0.0f;
			transformBuffer.positionAndRange[2] = 0.0f;
			transformBuffer.positionAndRange[3] = 1.0f;
			transformBuffer.isDirectional = TRUE;

			VertexConstantBuffer<ConstantBuffer> transformConstantBuffer{aGfx, 1u};
			transformConstantBuffer.Update(aGfx, transformBuffer);
			transformConstantBuffer.Bind(aGfx);

			quadVertexBuffer.Bind(aGfx);
			quadIndexBuffer.Bind(aGfx);

			directionalLightPS->Bind(aGfx);

			aGfx.DrawIndexed(6u);
		}

		{
			aGfx.SetBlendState(eBlendStates::Additive);
			aGfx.SetRasterizerState(eRasterizerStates::FrontfaceCulling);
			aGfx.SetDepthStencilState(eDepthStencilStates::ReadOnlyGreater);

			sphereVertexBuffer.Bind(aGfx);
			sphereIndexBuffer.Bind(aGfx);

			// Point Lights
			{
				pointLightPS->Bind(aGfx);

				for (PointLightData& lightData : pointLightData)
				{
					ConstantBuffer transformBuffer;

					transformBuffer.positionAndRange[0] = lightData.position.x;
					transformBuffer.positionAndRange[1] = lightData.position.y;
					transformBuffer.positionAndRange[2] = lightData.position.z;
					transformBuffer.positionAndRange[3] = lightData.radius;
					transformBuffer.isDirectional = FALSE;

					VertexConstantBuffer<ConstantBuffer> transformConstantBuffer{aGfx, 1u};
					transformConstantBuffer.Update(aGfx, transformBuffer);
					transformConstantBuffer.Bind(aGfx);

					PixelConstantBuffer<PointLightData> pointLightBuffer{aGfx, 2u};
					pointLightBuffer.Update(aGfx, lightData);
					pointLightBuffer.Bind(aGfx);

					aGfx.DrawIndexed(static_cast<UINT>(sphereIndexBuffer.GetCount()));
				}
			}

			// Spot Lights
			{
				spotLightPS->Bind(aGfx);

				for (SpotLightData& lightData : spotLightData)
				{
					ConstantBuffer transformBuffer;

					transformBuffer.positionAndRange[0] = lightData.position.x;
					transformBuffer.positionAndRange[1] = lightData.position.y;
					transformBuffer.positionAndRange[2] = lightData.position.z;
					transformBuffer.positionAndRange[3] = lightData.range;
					transformBuffer.isDirectional = FALSE;

					VertexConstantBuffer<ConstantBuffer> transformConstantBuffer{aGfx, 1u};
					transformConstantBuffer.Update(aGfx, transformBuffer);
					transformConstantBuffer.Bind(aGfx);

					PixelConstantBuffer<SpotLightData> spotLightBuffer{aGfx, 3u};
					spotLightBuffer.Update(aGfx, lightData);
					spotLightBuffer.Bind(aGfx);

					aGfx.DrawIndexed(static_cast<UINT>(sphereIndexBuffer.GetCount()));
				}
			}

			aGfx.SetBlendState(eBlendStates::Disabled);
			aGfx.SetDepthStencilState(eDepthStencilStates::Normal);
			aGfx.SetRasterizerState(eRasterizerStates::BackfaceCulling);
		}

		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[1u] = {nullptr};
		aGfx.pContext->PSSetShaderResources(0u, 1u, nullSRVs);
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

	PointLightData& DeferredLights::AddPointLight()
	{
		pointLightData.emplace_back();
		return pointLightData.back();
	}

	SpotLightData& DeferredLights::AddSpotLight()
	{
		spotLightData.emplace_back();
		return spotLightData.back();
	}
}
