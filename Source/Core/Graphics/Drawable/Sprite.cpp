#include "stdafx.h"
#include "Sprite.h"

#include "ModelLoader.h"
#include "Core/Graphics/Drawable/Vertex.h"
#include "External/include/imgui/imgui.h"

namespace Kaka
{
	void Sprite::Init(const Graphics& aGfx, float aSize)
	{
		constexpr float uvFactor = 1.0f;

		Vertex v0 = {};
		v0.position = DirectX::XMFLOAT3(-aSize, 0.0f, aSize);
		v0.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v0.texCoord = DirectX::XMFLOAT2(0.0f / uvFactor, 0.0f / uvFactor);
		v0.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v0.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v0);

		Vertex v1 = {};
		v1.position = DirectX::XMFLOAT3(aSize, 0.0f, aSize);
		v1.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v1.texCoord = DirectX::XMFLOAT2(0.0f / uvFactor, 1.0f / uvFactor);
		v1.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v1.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v1);

		Vertex v2 = {};
		v2.position = DirectX::XMFLOAT3(aSize, 0.0f, -aSize);
		v2.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v2.texCoord = DirectX::XMFLOAT2(1.0f / uvFactor, 1.0f / uvFactor);
		v2.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v2.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v2);

		Vertex v3 = {};
		v3.position = DirectX::XMFLOAT3(-aSize, 0.0f, -aSize);
		v3.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v3.texCoord = DirectX::XMFLOAT2(1.0f / uvFactor, 0.0f / uvFactor);
		v3.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v3.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v3);

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);

		texture = ModelLoader::LoadTexture(aGfx, "Assets\\Textures\\Water\\Water_c.jpg");

		sampler.Init(aGfx, 0u);

		vertexBuffer.Init(aGfx, vertices);
		indexBuffer.Init(aGfx, indices);

		vertexShader = ShaderFactory::GetVertexShader(aGfx, L"Shaders\\Sprite_VS.cso");
		pixelShader = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\Sprite_PS.cso");

		inputLayout.Init(aGfx, ied, vertexShader->GetBytecode());
		topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		rasterizer.Init(aGfx, eCullingMode::None);
		depthStencil.Init(aGfx, DepthStencil::Mode::Off);
	}

	void Sprite::Draw(Graphics& aGfx)
	{
		sampler.Bind(aGfx);
		texture->Bind(aGfx);

		vertexBuffer.Bind(aGfx);
		indexBuffer.Bind(aGfx);

		// Get the inverse of the camera matrix
		DirectX::XMMATRIX cameraInverse = DirectX::XMMatrixInverse(nullptr, aGfx.GetCurrentCameraMatrix());

		// Calculate the object's forward direction in camera space
		DirectX::XMVECTOR objectForwardCameraSpace = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), cameraInverse);

		// Calculate pitch and yaw angles
		DirectX::XMFLOAT3 angles;
		angles.y = atan2(DirectX::XMVectorGetX(objectForwardCameraSpace), DirectX::XMVectorGetZ(objectForwardCameraSpace));
		angles.x = asin(DirectX::XMVectorGetY(objectForwardCameraSpace));

		// Calculate the roll angle using the camera matrix directly
		DirectX::XMFLOAT4X4 cameraMatrix;
		DirectX::XMStoreFloat4x4(&cameraMatrix, aGfx.GetCurrentCameraMatrix());

		angles.z = atan2(cameraMatrix._21, cameraMatrix._11);

		// Set the rotation angles
		transform.pitch = angles.y;
		transform.yaw = angles.x;
		transform.roll = angles.z;

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		pixelShader->Bind(aGfx);

		PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
		psConstantBuffer.Bind(aGfx);

		vertexShader->Bind(aGfx);
		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);
		rasterizer.Bind(aGfx);
		depthStencil.Bind(aGfx);

		aGfx.DrawIndexed(static_cast<UINT>(std::size(indices)));

		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[4] = {nullptr};
		aGfx.pContext->PSSetShaderResources(2u, 4u, nullSRVs);
	}

	void Sprite::SetPosition(const DirectX::XMFLOAT3 aPosition)
	{
		transform.x = aPosition.x;
		transform.y = aPosition.y;
		transform.z = aPosition.z;
	}

	void Sprite::SetScale(const float aScale)
	{
		transform.scale = aScale;
	}

	DirectX::XMMATRIX Sprite::GetTransform() const
	{
		return DirectX::XMMatrixScaling(transform.scale, transform.scale, transform.scale) *
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.roll) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	DirectX::XMFLOAT3 Sprite::GetPosition() const
	{
		return {transform.x, transform.y, transform.z};
	}

	void Sprite::ShowControlWindow(const char* aWindowName)
	{
		aWindowName = aWindowName ? aWindowName : "Sprite";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::DragFloat3("XYZ", &transform.x);
			ImGui::Text("Scale");
			ImGui::DragFloat("XYZ", &transform.scale, 0.1f, 0.0f, 10.0f, "%.1f");
		}
		ImGui::End();
	}
}
