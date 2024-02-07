#include "stdafx.h"
#include "Sprite.h"

#include "ModelLoader.h"
#include "Core/Graphics/Drawable/Vertex.h"
#include "External/include/imgui/imgui.h"
#include "Utility/Camera.h"

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

		texture = ModelLoader::LoadTexture(aGfx, "Assets\\Textures\\SpriteCloud.png");

		sampler.Init(aGfx, 0u);

		vertexBuffer.Init(aGfx, vertices);
		indexBuffer.Init(aGfx, indices);

		vertexShader = ShaderFactory::GetVertexShader(aGfx, L"Shaders\\Sprite_VS.cso");
		pixelShader = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\Sprite_PS.cso");

		inputLayout.Init(aGfx, ied, vertexShader->GetBytecode());
		topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		rasterizer.Init(aGfx, eCullingMode::None);
		depthStencil.Init(aGfx, DepthStencil::Mode::DepthFirst);

		mTransform = DirectX::XMMatrixIdentity();
	}

	void Sprite::Draw(Graphics& aGfx)
	{
		sampler.Bind(aGfx);
		texture->Bind(aGfx);

		vertexBuffer.Bind(aGfx);
		indexBuffer.Bind(aGfx);

		// Rotate the sprite to face the camera
		const DirectX::XMVECTOR cameraForward = aGfx.camera->GetForwardVector();

		// Get right vector from cross product of forward and up
		DirectX::XMVECTOR cameraRight = DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraForward);
		cameraRight = DirectX::XMVector3Normalize(cameraRight);
		DirectX::XMVECTOR cameraUp = DirectX::XMVector3Cross(cameraForward, cameraRight);
		cameraUp = DirectX::XMVector3Normalize(cameraUp);

		// X
		mTransform.r[0].m128_f32[0] = cameraRight.m128_f32[0];
		mTransform.r[0].m128_f32[1] = cameraRight.m128_f32[1];
		mTransform.r[0].m128_f32[2] = cameraRight.m128_f32[2];

		// Y
		mTransform.r[1].m128_f32[0] = cameraForward.m128_f32[0];
		mTransform.r[1].m128_f32[1] = cameraForward.m128_f32[1];
		mTransform.r[1].m128_f32[2] = cameraForward.m128_f32[2];

		// Z
		mTransform.r[2].m128_f32[0] = cameraUp.m128_f32[0];
		mTransform.r[2].m128_f32[1] = cameraUp.m128_f32[1];
		mTransform.r[2].m128_f32[2] = cameraUp.m128_f32[2];

		// Apply rotation so that the sprite can spin around the axis pointing towards the camera
		mTransform = DirectX::XMMatrixRotationRollPitchYaw(0.0f, rotation, 0.0f) * mTransform;

		DirectX::XMFLOAT3 position = GetPosition();
		mTransform *= DirectX::XMMatrixScaling(transform.scale, transform.scale, transform.scale);
		SetPosition(position);

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
		// Set position directly in transform
		mTransform.r[3] = DirectX::XMVectorSet(aPosition.x, aPosition.y, aPosition.z, 1.0f);
		transform.x = aPosition.x;
		transform.y = aPosition.y;
		transform.z = aPosition.z;
	}

	void Sprite::SetScale(const float aScale)
	{
		transform.scale = aScale;
		DirectX::XMFLOAT3 position = GetPosition();
		mTransform *= DirectX::XMMatrixScaling(aScale, aScale, aScale);
		SetPosition(position);
	}

	void Sprite::SetRotation(float aRotation)
	{
		rotation = aRotation;
	}

	DirectX::XMMATRIX Sprite::GetTransform() const
	{
		return mTransform;
	}

	DirectX::XMFLOAT3 Sprite::GetPosition() const
	{
		DirectX::XMFLOAT3 position = {
			mTransform.r[3].m128_f32[0],
			mTransform.r[3].m128_f32[1],
			mTransform.r[3].m128_f32[2]
		};
		return position;
	}

	void Sprite::ShowControlWindow(const char* aWindowName)
	{
		aWindowName = aWindowName ? aWindowName : "Sprite";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Orientation");
			ImGui::SliderAngle("X", mTransform.r[0].m128_f32 + 0, -180.0f, 180.0f);
			ImGui::SliderAngle("Y", mTransform.r[0].m128_f32 + 1, -180.0f, 180.0f);
			ImGui::SliderAngle("Z", mTransform.r[0].m128_f32 + 2, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::DragFloat3("XYZ", mTransform.r[3].m128_f32);
			ImGui::Text("Scale");
			ImGui::DragFloat("XYZ", mTransform.r[4].m128_f32, 0.1f, 0.0f, 10.0f, "%.1f");
		}
		ImGui::End();
	}
}
