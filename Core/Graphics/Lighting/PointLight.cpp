#include "PointLight.h"
#include <External/include/imgui/imgui.h>
#include <array>

namespace Kaka
{
	PointLight::PointLight(const Graphics& aGfx, const UINT aSlot)
		:
		cBuffer(aGfx, aSlot)
	{
		index = sharedIndex;
		assert(sharedIndex < MAX_LIGHTS && "Too many point lights!");
		pointLightData.emplace_back();
		sharedIndex++;

		Reset();
	}

	void PointLight::Bind(const Graphics& aGfx, DirectX::FXMMATRIX aView)
	{
		PointLightBuffer dataCopy;
		dataCopy.activeLights = sharedIndex;
		for (UINT i = 0; i < dataCopy.activeLights; ++i)
		{
			dataCopy.plb[i] = pointLightData[i];
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&pointLightData[i].position);
			XMStoreFloat3(&dataCopy.plb[i].position, XMVector3Transform(pos, aView));
		}
		cBuffer.Update(aGfx, dataCopy);
		cBuffer.Bind(aGfx);
	}

	void PointLight::ShowControlWindow(const char* aWindowName) const
	{
		aWindowName = aWindowName ? aWindowName : "Point Light";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Position");
			ImGui::DragFloat3("XYZ", &pointLightData[index].position.x, 1.0f);
			//ImGui::DragFloat("Y", &pointLightData[index].position.x, 1.0f);
			//ImGui::DragFloat("Z", &pointLightData[index].position.x, 1.0f);
			//ImGui::SliderFloat("Y", &pointLightData[index].position.y, -20.0f, 20.0f, "%.1f");
			//ImGui::SliderFloat("Z", &pointLightData[index].position.z, -20.0f, 20.0f, "%.1f");

			ImGui::Text("Intensity/Colour");
			ImGui::SliderFloat("Intensity", &pointLightData[index].diffuseIntensity, 0.01f, 4.0f, "%.2f");
			ImGui::ColorEdit3("Diffuse Colour", &pointLightData[index].diffuseColour.x);

			ImGui::Text("Falloff");
			ImGui::SliderFloat("Constant", &pointLightData[index].attConst, 0.05f, 10.0f, "%.2f",
			                   ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Linear", &pointLightData[index].attLin, 0.0001f, 4.0f, "%.4f",
			                   ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Quadratic", &pointLightData[index].attQuad, 0.0000001f, 10.0f, "%.7f",
			                   ImGuiSliderFlags_Logarithmic);

			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
		ImGui::End();
	}

	DirectX::XMMATRIX PointLight::GetTransform() const
	{
		return DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0) *
			DirectX::XMMatrixTranslation(pointLightData[index].position.x, pointLightData[index].position.y,
			                             pointLightData[index].position.z);
	}

	void PointLight::SetPosition(const DirectX::XMFLOAT3 aPosition) const
	{
		pointLightData[index].position = aPosition;
	}

	void PointLight::SetColour(const DirectX::XMFLOAT3 aColour) const
	{
		pointLightData[index].diffuseColour = aColour;
	}

	void PointLight::SetIntensity(const float aIntensity) const
	{
		pointLightData[index].diffuseIntensity = aIntensity;
	}

	void PointLight::Reset() const
	{
		pointLightData[index].position = {0.0f,2.0f,0.0f};
		pointLightData[index].diffuseColour = {1.0f,1.0f,1.0f};
		pointLightData[index].diffuseIntensity = 2.0f;
		pointLightData[index].attConst = 1.0f;
		pointLightData[index].attLin = 0.02f;
		pointLightData[index].attQuad = 0.00055f;
		pointLightData[index].active = true;
	}

	void PointLight::Draw(const Graphics& aGfx) const
	{
		const std::vector<Vertex> vertices = {
			{{-0.1f,0.0f,0.0f}},
			{{0.1f,0.0f,0.0f}},
			{{0.0f,-0.1f,0.0f}},
			{{0.0f,0.1f,0.0f}},
			{{0.0f,0.0f,-0.1f}},
			{{0.0f,0.0f,0.1f}}
		};
		VertexBuffer vertexBuffer(aGfx, vertices);
		vertexBuffer.Bind(aGfx);

		const std::vector<unsigned short> indices =
		{
			0,2,4,
			0,4,3,
			0,3,5,
			0,5,2,
			1,4,2,
			1,3,4,
			1,5,3,
			1,2,5
		};

		IndexBuffer indexBuffer(aGfx, indices);
		indexBuffer.Bind(aGfx);

		// Create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelProjection;
		};
		const DirectX::XMMATRIX modelView = GetTransform() * aGfx.GetCamera();

		const ConstantBuffer cb =
		{
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView * aGfx.GetProjection())
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cb);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &cb;
		aGfx.pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);

		// Bind constant buffer to vertex shader
		aGfx.pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

		PixelShader pixelShader(aGfx, L"Shaders\\Solid_PS.cso");
		pixelShader.Bind(aGfx);

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT4 colour;
		} pmc;
		pmc.colour = {
			pointLightData[index].diffuseColour.x,
			pointLightData[index].diffuseColour.y,
			pointLightData[index].diffuseColour.z,
			1.0f
		};

		PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
		psConstantBuffer.Bind(aGfx);

		VertexShader vertexShader(aGfx, L"Shaders\\Solid_VS.cso");
		vertexShader.Bind(aGfx);


		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{
				"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},
		};
		InputLayout inputLayout(aGfx, ied, vertexShader.GetBytecode());
		inputLayout.Bind(aGfx);


		Topology topology(aGfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		topology.Bind(aGfx);

		aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
	}
}