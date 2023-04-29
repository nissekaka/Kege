#include "Model.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Model/MeshLoader.h"
#include <External/include/imgui/imgui.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

namespace Kaka
{
	Model::Model(const Graphics& aGfx, const std::string& aFilePath)
	{
		MeshLoader::LoadMesh(aFilePath, mesh);
		texture.LoadTexture(aGfx, aFilePath);
	}

	void Model::Draw(const Graphics& aGfx)
	{
		Sampler sampler(aGfx, 0u);
		sampler.Bind(aGfx);

		texture.Bind(aGfx);

		VertexBuffer vertexBuffer(aGfx, mesh.vertices);
		vertexBuffer.Bind(aGfx);

		IndexBuffer indexBuffer(aGfx, mesh.indices);
		indexBuffer.Bind(aGfx);

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		PixelShader pixelShader(aGfx, L"Shaders\\Light_PS.cso");
		pixelShader.Bind(aGfx);

		VertexShader vertexShader(aGfx, L"Shaders\\Light_VS.cso");
		vertexShader.Bind(aGfx);

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
		};
		InputLayout inputLayout(aGfx, ied, vertexShader.GetBytecode());
		inputLayout.Bind(aGfx);

		Topology topology(aGfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		topology.Bind(aGfx);

		aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(mesh.indices)), 0u, 0u);
	}

	void Model::SetPosition(const DirectX::XMFLOAT3 aPosition)
	{
		transform.x = aPosition.x;
		transform.y = aPosition.y;
		transform.z = aPosition.z;
	}

	void Model::SetRotation(const DirectX::XMFLOAT3 aRotation)
	{
		transform.roll = aRotation.x;
		transform.pitch = aRotation.y;
		transform.yaw = aRotation.z;
	}

	void Model::SetScale(const float aScale)
	{
		transform.scale = aScale;
	}

	DirectX::XMFLOAT3 Model::GetPosition() const
	{
		return {transform.x, transform.y, transform.z};
	}

	DirectX::XMFLOAT3 Model::GetRotation() const
	{
		return {transform.roll, transform.pitch, transform.yaw};
	}

	DirectX::XMMATRIX Model::GetTransform() const
	{
		// TODO Rotates faster when scale is low, scale should only affect size
		return DirectX::XMMatrixScaling(transform.scale, transform.scale, transform.scale) *
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	float Model::GetScale() const
	{
		return transform.scale;
	}

	void Model::ShowControlWindow(const char* aWindowName)
	{
		// Window name defaults to "Model"
		aWindowName = aWindowName ? aWindowName : "Model";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Columns(2, nullptr, true);
			ImGui::NextColumn();
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			ImGui::Text("Scale");
			ImGui::SliderFloat("XYZ", &transform.scale, 0.0f, 5.0f, nullptr, ImGuiSliderFlags_Logarithmic);
		}
		ImGui::End();
	}
}
