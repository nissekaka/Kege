#include "stdafx.h"
#include "Sprite.h"

#include <random>

#include "ModelLoader.h"
#include "Core/Graphics/Drawable/Vertex.h"
#include "Core/Utility/Camera.h"
#include "Core/Utility/KakaMath.h"
#include <External/include/imgui/imgui.h>

namespace Kaka
{
	void Sprite::Init(const Graphics& aGfx, const float aSize, const unsigned int aNumberOfSprites, bool aIsVfx, const std::string& aFile)
	{
		constexpr float uvFactor = 1.0f;

		SpriteVertex v0 = {};
		v0.position = DirectX::XMFLOAT4(-aSize, 0.0f, aSize, 1.0f);
		v0.texCoord = DirectX::XMFLOAT2(0.0f / uvFactor, 0.0f / uvFactor);
		v0.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v0.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v0.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v0);

		SpriteVertex v1 = {};
		v1.position = DirectX::XMFLOAT4(aSize, 0.0f, aSize, 1.0f);
		v1.texCoord = DirectX::XMFLOAT2(0.0f / uvFactor, 1.0f / uvFactor);
		v1.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v1.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v1.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v1);

		SpriteVertex v2 = {};
		v2.position = DirectX::XMFLOAT4(aSize, 0.0f, -aSize, 1.0f);
		v2.texCoord = DirectX::XMFLOAT2(1.0f / uvFactor, 1.0f / uvFactor);
		v2.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v2.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v2.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v2);

		SpriteVertex v3 = {};
		v3.position = DirectX::XMFLOAT4(-aSize, 0.0f, -aSize, 1.0f);
		v3.texCoord = DirectX::XMFLOAT2(1.0f / uvFactor, 0.0f / uvFactor);
		v3.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		v3.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		v3.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices.push_back(v3);

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);

		texture = ModelLoader::LoadTexture(aGfx, aFile, 1u);

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(SpriteVertex) * 4;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = vertices.data();

		ID3D11Device* device = aGfx.pDevice.Get();
		HRESULT result = device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);
		assert(SUCCEEDED(result));

		//vertexBuffer.Init(aGfx, vertices);
		indexBuffer.Init(aGfx, indices);

		// Create instance buffer
		D3D11_BUFFER_DESC instanceBufferDesc = {};

		instanceBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX) * 24576;
		// 8192 + 8192 + 8192 = 24576
		instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		result = aGfx.pDevice->CreateBuffer(&instanceBufferDesc, nullptr, &instanceBuffer);
		assert(SUCCEEDED(result));

		//D3D11_BUFFER_DESC spriteRenderBufferDesc = {};

		//spriteRenderBufferDesc.ByteWidth = sizeof(SpriteRenderBuffer);
		//spriteRenderBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		//spriteRenderBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		//spriteRenderBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		//result = aGfx.pDevice->CreateBuffer(&spriteRenderBufferDesc, nullptr, &spriteRenderBuffer);
		//assert(SUCCEEDED(result));

		vertexShader = ShaderFactory::GetVertexShader(aGfx, L"Shaders\\Sprite_VS.cso");

		vfxPixelShader = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\Sprite_PS.cso");
		deferredPixelShader = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\Sprite_Deferred_PS.cso");

		if (aIsVfx)
		{
			pixelShader = vfxPixelShader;
		}
		else
		{
			pixelShader = deferredPixelShader;
		}

		inputLayout.Init(aGfx, ied, vertexShader->GetBytecode());

		transforms.resize(aNumberOfSprites);
		travelAngles.resize(aNumberOfSprites);
		travelSpeeds.resize(aNumberOfSprites);
		travelRadiuses.resize(aNumberOfSprites);

		std::random_device rd;
		std::mt19937 gen(rd());

		for (unsigned int i = 0; i < transforms.size(); ++i)
		{
			// Set random position between -20, 0, -20 and 20, 0, 20
			std::uniform_real_distribution<float> xDist(-150.0f, 130.0f);
			std::uniform_real_distribution<float> yDist(0.0f, 150.0f);
			std::uniform_real_distribution<float> zDist(-70.0f, 70.0f);
			std::uniform_real_distribution<float> distSize(0.5f, 2.0f);
			float scale = distSize(gen);
			transforms[i] = DirectX::XMMatrixScaling(scale, scale, scale);

			SetPosition({xDist(gen), yDist(gen), zDist(gen)}, i);
			startPositions.push_back(GetPosition(i));

			std::uniform_real_distribution<float> radiusDist(0.5f, 10.0f);
			std::uniform_real_distribution<float> speedDist(0.1f, 0.5f);
			travelRadiuses[i] = radiusDist(gen);
			travelSpeeds[i] = speedDist(gen);
			travelAngles[i] = PI;
		}
	}

	void Sprite::Draw(Graphics& aGfx)
	{
		texture->Bind(aGfx);

		unsigned int strides[2];
		unsigned int offsets[2];
		ID3D11Buffer* bufferPointers[2];

		strides[0] = sizeof(SpriteVertex);
		strides[1] = sizeof(DirectX::XMMATRIX);

		offsets[0] = 0;
		offsets[1] = 0;

		bufferPointers[0] = vertexBuffer;
		bufferPointers[1] = instanceBuffer;

		aGfx.pContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

		indexBuffer.Bind(aGfx);
		inputLayout.Bind(aGfx);

		const unsigned int instanceCount = transforms.size();

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		aGfx.pContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(
			mappedResource.pData,
			transforms.data(),
			sizeof(DirectX::XMMATRIX) * instanceCount
		);
		aGfx.pContext->Unmap(instanceBuffer, 0);

		vertexShader->Bind(aGfx);
		if (aGfx.HasPixelShaderOverride())
		{
			aGfx.GetPixelShaderOverride()->Bind(aGfx);
		}
		else
		{
			pixelShader->Bind(aGfx);
		}

		aGfx.DrawIndexedInstanced(6u, instanceCount);

		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[4] = {nullptr};
		aGfx.pContext->PSSetShaderResources(2u, 4u, nullSRVs);
	}

	void Sprite::SetPosition(const DirectX::XMFLOAT3 aPosition, const unsigned int aIndex)
	{
		// Set position directly in transform
		transforms[aIndex].r[3] = DirectX::XMVectorSet(aPosition.x, aPosition.y, aPosition.z, 1.0f);
	}

	void Sprite::SetScale(const float aScale, const unsigned int aIndex)
	{
		DirectX::XMFLOAT3 position = GetPosition(aIndex);
		transforms[aIndex] *= DirectX::XMMatrixScaling(aScale, aScale, aScale);
		SetPosition(position, aIndex);
	}

	//void Sprite::SetRotation(float aRotation, const unsigned int aIndex)
	//{
	//	rotations[aIndex] = aRotation;
	//}

	DirectX::XMMATRIX Sprite::GetTransform() const
	{
		return DirectX::XMMatrixIdentity();
		//return transforms;
	}

	DirectX::XMFLOAT3 Sprite::GetPosition(unsigned int aIndex) const
	{
		DirectX::XMFLOAT3 position = {
			transforms[aIndex].r[3].m128_f32[0],
			transforms[aIndex].r[3].m128_f32[1],
			transforms[aIndex].r[3].m128_f32[2]
		};
		return position;
	}

	void Sprite::ShowControlWindow(const char* aWindowName)
	{
		//aWindowName = aWindowName ? aWindowName : "Sprite";

		//if (ImGui::Begin(aWindowName))
		//{
		//	ImGui::Text("Orientation");
		//	ImGui::SliderAngle("X", transforms.r[0].m128_f32 + 0, -180.0f, 180.0f);
		//	ImGui::SliderAngle("Y", transforms.r[0].m128_f32 + 1, -180.0f, 180.0f);
		//	ImGui::SliderAngle("Z", transforms.r[0].m128_f32 + 2, -180.0f, 180.0f);
		//	ImGui::Text("Position");
		//	ImGui::DragFloat3("XYZ", transforms.r[3].m128_f32);
		//	ImGui::Text("Scale");
		//	ImGui::DragFloat("XYZ", transforms.r[4].m128_f32, 0.1f, 0.0f, 10.0f, "%.1f");
		//}
		//ImGui::End();
	}

	void Sprite::Update(const Graphics& aGfx, const float aDeltaTime)
	{
		const unsigned int instanceCount = transforms.size();

		// Rotate the sprite to face the camera
		const DirectX::XMVECTOR cameraForward = aGfx.camera->GetForwardVector();

		// Get right vector from cross product of forward and up
		DirectX::XMVECTOR cameraRight = DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraForward);
		cameraRight = DirectX::XMVector3Normalize(cameraRight);
		DirectX::XMVECTOR cameraUp = DirectX::XMVector3Cross(cameraForward, cameraRight);
		cameraUp = DirectX::XMVector3Normalize(cameraUp);


		for (; updateIndex < updateCounter; ++updateIndex)
		{
			travelAngles[updateIndex] -= travelSpeeds[updateIndex] * aDeltaTime;

			transforms[updateIndex].r[3].m128_f32[0] = travelRadiuses[updateIndex] * std::cos(travelAngles[updateIndex]) + startPositions[updateIndex].x;
			transforms[updateIndex].r[3].m128_f32[2] = travelRadiuses[updateIndex] * std::sin(travelAngles[updateIndex]) + startPositions[updateIndex].z;

			if (travelAngles[updateIndex] > 2 * PI)
			{
				travelAngles[updateIndex] -= 2 * PI;
			}

			// Set the rotation to face the camera
			transforms[updateIndex].r[0] = cameraRight;
			transforms[updateIndex].r[1] = cameraForward;
			transforms[updateIndex].r[2] = cameraUp;

			transforms[updateIndex] = transforms[updateIndex];
		}

		updateCounter += updateIncrease;

		if (updateCounter >= instanceCount)
		{
			updateCounter = 0;
			updateIndex = 0;
		}
	}
}
