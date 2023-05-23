#include "Terrain.h"
#include "Core/Model/Vertex.h"

#include "TGP/uppgift05_helper.h"
#include "TGP/FastNoiseLite.h"

#include <cmath>
#include <random>

namespace Kaka
{
	float GetHeight(const float aX, const float aZ)
	{
		return 0.1f * (aZ * sinf(0.4f * aX) + aX * cosf(0.4f * aZ));
	}

	void Terrain::Init(const Graphics& aGfx, const int aSize)
	{
		FastNoiseLite noiseGenerator;
		noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noiseGenerator.SetFractalType(FastNoiseLite::FractalType_Ridged);
		noiseGenerator.SetFrequency(0.005f);
		noiseGenerator.SetFractalOctaves(3);
		noiseGenerator.SetFractalLacunarity(2.0f);
		noiseGenerator.SetFractalGain(0.5f);

		for (int z = 0; z < aSize; ++z)
		{
			for (int x = 0; x < aSize; ++x)
			{
				Vertex vertex;
				float noiseValue = noiseGenerator.GetNoise(static_cast<float>(x), static_cast<float>(z));
				vertex.position = {static_cast<float>(x),noiseValue * 25.0f,static_cast<float>(z)};
				vertex.normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
				vertex.texCoord = {static_cast<float>(x) / 8.0f,static_cast<float>(z) / 8.0f};
				vertex.tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
				vertex.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
				tVertices.push_back(vertex);
			}
		}

		// Calculate normals
		for (int z = 0; z < aSize; ++z)
		{
			for (int x = 0; x < aSize; ++x)
			{
				const int currentIndex = z * aSize + x;
				const int indexTop = (z - 1) * aSize + x;
				const int indexBottom = (z + 1) * aSize + x;
				const int indexLeft = z * aSize + (x - 1);
				const int indexRight = z * aSize + (x + 1);

				const bool isBorder = (x == 0 || x == aSize - 1 || z == 0 || z == aSize - 1);

				DirectX::XMFLOAT3& currentPosition = tVertices[currentIndex].position;
				DirectX::XMFLOAT3 topPosition = (z > 0) ? tVertices[indexTop].position : currentPosition;
				DirectX::XMFLOAT3 bottomPosition = (z < aSize - 1) ? tVertices[indexBottom].position : currentPosition;
				DirectX::XMFLOAT3 leftPosition = (x > 0) ? tVertices[indexLeft].position : currentPosition;
				DirectX::XMFLOAT3 rightPosition = (x < aSize - 1) ? tVertices[indexRight].position : currentPosition;

				DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(XMLoadFloat3(&topPosition),
				                                                    XMLoadFloat3(&bottomPosition));
				DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(XMLoadFloat3(&leftPosition),
				                                                    XMLoadFloat3(&rightPosition));
				DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge1, edge2);

				normal = DirectX::XMVector3Normalize(normal);
				DirectX::XMStoreFloat3(&tVertices[currentIndex].normal, normal);

				if (isBorder)
				{
					DirectX::XMStoreFloat3(&tVertices[currentIndex].normal, {0,1,0});
				}
				else
				{
					edge1 = DirectX::XMVectorSubtract(XMLoadFloat3(&leftPosition), XMLoadFloat3(&currentPosition));
					edge2 = DirectX::XMVectorSubtract(XMLoadFloat3(&bottomPosition), XMLoadFloat3(&currentPosition));

					DirectX::XMFLOAT2 deltaUV1 = DirectX::XMFLOAT2(1.0f, 0.0f);
					DirectX::XMFLOAT2 deltaUV2 = DirectX::XMFLOAT2(0.0f, 1.0f);

					// Calculate tangent
					DirectX::XMVECTOR tangent;
					tangent = DirectX::XMVectorScale(DirectX::XMVectorSubtract(DirectX::XMVectorScale(edge1, deltaUV2.y), DirectX::XMVectorScale(edge2, deltaUV1.y)),
					                                 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y));
					tangent = DirectX::XMVector3Normalize(tangent);

					// Calculate bitangent
					DirectX::XMVECTOR bitangent;
					bitangent = DirectX::XMVectorScale(DirectX::XMVectorSubtract(DirectX::XMVectorScale(edge2, deltaUV1.x), DirectX::XMVectorScale(edge1, deltaUV2.x)),
					                                   1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y));
					bitangent = DirectX::XMVector3Normalize(bitangent);

					DirectX::XMStoreFloat3(&tVertices[currentIndex].tangent, tangent);
					DirectX::XMStoreFloat3(&tVertices[currentIndex].bitangent, bitangent);
				}
			}
		}

		for (int z = 0; z < aSize - 1; ++z)
		{
			for (int x = 0; x < aSize - 1; ++x)
			{
				const int index = z * aSize + x;

				tIndices.push_back(static_cast<unsigned short>(index));
				tIndices.push_back(static_cast<unsigned short>(index + aSize));
				tIndices.push_back(static_cast<unsigned short>(index + 1));

				tIndices.push_back(static_cast<unsigned short>(index + aSize));
				tIndices.push_back(static_cast<unsigned short>(index + aSize + 1));
				tIndices.push_back(static_cast<unsigned short>(index + 1));
			}
		}

		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Grass\\Stylized_Grass_003_BaseColor.jpg");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Grass\\Stylized_Grass_003_Normal.jpg");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Dirt\\Stylized_Dry_Mud_001_BaseColor.jpg");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Dirt\\Stylized_Dry_Mud_001_Normal.jpg");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Rock\\peter-larsen-stylizedrockdiffuse.jpg");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Rock\\peter-larsen-stylizedrocknormal-png.jpg");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Snow\\Snow_001_BaseColor.jpg");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Snow\\Snow_001_Normal.jpg");
	}

	void Terrain::Draw(const Graphics& aGfx)
	{
		Sampler sampler(aGfx, 0u);
		sampler.Bind(aGfx);

		texture.Bind(aGfx);

		VertexBuffer vb(aGfx, tVertices);
		vb.Bind(aGfx);

		IndexBuffer ib(aGfx, tIndices);
		ib.Bind(aGfx);

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		PixelShader pixelShader(aGfx, L"Shaders\\TerrainPhong_PS.cso");
		pixelShader.Bind(aGfx);

		const struct PSMaterialConstant
		{
			BOOL normalMapEnabled = TRUE;
			BOOL materialEnabled = TRUE;
			float specularIntensity = 0.1f;
			float specularPower = 500.0f;
		} pmc;

		PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
		psConstantBuffer.Bind(aGfx);

		VertexShader vertexShader(aGfx, L"Shaders\\TerrainPhong_VS.cso");
		vertexShader.Bind(aGfx);

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{
				"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},
			{
				"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},
			{
				"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},
			{
				"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},
			{
				"BITANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},
		};
		InputLayout inputLayout(aGfx, ied, vertexShader.GetBytecode());
		inputLayout.Bind(aGfx);

		Topology topology(aGfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		topology.Bind(aGfx);

		aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(tIndices)), 0u, 0u);
	}

	DirectX::XMMATRIX Terrain::GetTransform() const
	{
		return DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0) *
			DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	}
}