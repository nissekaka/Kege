#include "Terrain.h"
#include "Core/Graphics/Drawable/Vertex.h"
#include "External/include/imgui/imgui.h"

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
		noiseGenerator.SetFrequency(0.0012f);
		noiseGenerator.SetFractalOctaves(8);
		noiseGenerator.SetFractalLacunarity(1.8f);
		noiseGenerator.SetFractalGain(0.5f);

		constexpr float heightMul = 100.0f;
		constexpr float uvFactor = 80.0f;

		std::vector<Vertex> terrainVertices;
		std::vector<unsigned short> terrainIndices;

		for (int z = 0; z < aSize; ++z)
		{
			for (int x = 0; x < aSize; ++x)
			{
				Vertex vertex = {};
				float noiseValue = noiseGenerator.GetNoise(static_cast<float>(x), static_cast<float>(z));
				vertex.position = DirectX::XMFLOAT3(static_cast<float>(x), noiseValue * heightMul,
				                                    static_cast<float>(z));
				vertex.normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
				vertex.texCoord = DirectX::XMFLOAT2(static_cast<float>(x) / uvFactor,
				                                    static_cast<float>(z) / uvFactor);
				vertex.tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
				vertex.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
				terrainVertices.push_back(vertex);
			}
		}

		OutputDebugStringA("\nDone generating...");

		constexpr float smoothingFactor = 1.0f;
		float heightThreshold = 0.0f;
		int smoothingIterations = 15;

		for (int i = 0; i < smoothingIterations; ++i)
		{
			for (int z = 0; z < aSize; ++z)
			{
				for (int x = 0; x < aSize; ++x)
				{
					const int currentIndex = z * aSize + x;
					const int indexTop = (z - 1) * aSize + x;
					const int indexBottom = (z + 1) * aSize + x;
					const int indexLeft = z * aSize + (x - 1);
					const int indexRight = z * aSize + (x + 1);

					if (terrainVertices[currentIndex].position.y > heightThreshold)
					{
						continue;
					}

					// Calculate average position of neighbouring vertices
					DirectX::XMFLOAT3 averagePosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
					int numNeighbours = 0;

					if (z > 0)
					{
						averagePosition.y += terrainVertices[indexTop].position.y;
						numNeighbours++;
					}
					if (z < aSize - 1)
					{
						averagePosition.y += terrainVertices[indexBottom].position.y;
						numNeighbours++;
					}
					if (x > 0)
					{
						averagePosition.y += terrainVertices[indexLeft].position.y;
						numNeighbours++;
					}
					if (x < aSize - 1)
					{
						averagePosition.y += terrainVertices[indexRight].position.y;
						numNeighbours++;
					}
					averagePosition.y /= numNeighbours;

					DirectX::XMFLOAT3& currentPosition = terrainVertices[currentIndex].position;
					DirectX::XMFLOAT3 smoothedPosition = DirectX::XMFLOAT3(
						currentPosition.x,
						currentPosition.y + (averagePosition.y - currentPosition.y) * smoothingFactor,
						currentPosition.z
					);

					// Update the vertex position
					terrainVertices[currentIndex].position = smoothedPosition;
				}
			}
		}

		OutputDebugStringA("\nDone smoothing...");

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

				DirectX::XMFLOAT3& currentPosition = terrainVertices[currentIndex].position;
				DirectX::XMFLOAT3 topPosition = (z > 0) ? terrainVertices[indexTop].position : currentPosition;
				DirectX::XMFLOAT3 bottomPosition = (z < aSize - 1)
					                                   ? terrainVertices[indexBottom].position
					                                   : currentPosition;
				DirectX::XMFLOAT3 leftPosition = (x > 0) ? terrainVertices[indexLeft].position : currentPosition;
				DirectX::XMFLOAT3 rightPosition = (x < aSize - 1)
					                                  ? terrainVertices[indexRight].position
					                                  : currentPosition;

				DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(XMLoadFloat3(&topPosition),
				                                                    XMLoadFloat3(&bottomPosition));
				DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(XMLoadFloat3(&leftPosition),
				                                                    XMLoadFloat3(&rightPosition));
				DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge1, edge2);

				normal = DirectX::XMVector3Normalize(normal);
				DirectX::XMStoreFloat3(&terrainVertices[currentIndex].normal, normal);

				if (isBorder)
				{
					DirectX::XMStoreFloat3(&terrainVertices[currentIndex].normal, {0, 1, 0});
				}
				else
				{
					edge1 = DirectX::XMVectorSubtract(XMLoadFloat3(&leftPosition), XMLoadFloat3(&currentPosition));
					edge2 = DirectX::XMVectorSubtract(XMLoadFloat3(&bottomPosition), XMLoadFloat3(&currentPosition));

					DirectX::XMFLOAT2 deltaUV1 = DirectX::XMFLOAT2(1.0f, 0.0f);
					DirectX::XMFLOAT2 deltaUV2 = DirectX::XMFLOAT2(0.0f, 1.0f);

					// Calculate tangent
					DirectX::XMVECTOR tangent;
					tangent = DirectX::XMVectorScale(
						DirectX::XMVectorSubtract(DirectX::XMVectorScale(edge1, deltaUV2.y),
						                          DirectX::XMVectorScale(edge2, deltaUV1.y)),
						1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y));
					tangent = DirectX::XMVector3Normalize(tangent);

					// Calculate bitangent
					DirectX::XMVECTOR bitangent;
					bitangent = DirectX::XMVectorScale(
						DirectX::XMVectorSubtract(DirectX::XMVectorScale(edge2, deltaUV1.x),
						                          DirectX::XMVectorScale(edge1, deltaUV2.x)),
						1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y));
					bitangent = DirectX::XMVector3Normalize(bitangent);

					DirectX::XMStoreFloat3(&terrainVertices[currentIndex].tangent, tangent);
					DirectX::XMStoreFloat3(&terrainVertices[currentIndex].bitangent, bitangent);
				}
			}
		}

		OutputDebugStringA("\nDone with normals...");

		for (int z = 0; z < aSize - 1; ++z)
		{
			for (int x = 0; x < aSize - 1; ++x)
			{
				const int index = z * aSize + x;

				terrainIndices.push_back(static_cast<unsigned short>(index));
				terrainIndices.push_back(static_cast<unsigned short>(index + aSize));
				terrainIndices.push_back(static_cast<unsigned short>(index + 1));

				terrainIndices.push_back(static_cast<unsigned short>(index + aSize));
				terrainIndices.push_back(static_cast<unsigned short>(index + aSize + 1));
				terrainIndices.push_back(static_cast<unsigned short>(index + 1));
			}
		}

		OutputDebugStringA("\nPushed indices...");

		constexpr int subsetSize = 62500;
		const int numVertices = static_cast<int>(terrainVertices.size());
		const int numSubsets = (numVertices + subsetSize - 1) / subsetSize; // Round up

		terrainSubsets.resize(numSubsets);

		for (int i = 0; i < numSubsets; ++i)
		{
			const int startIndex = i * subsetSize;
			const int endIndex = (std::min)(startIndex + subsetSize, numVertices);

			// Add the last row's vertices to the next subset's first row
			if (i < numSubsets - 1)
			{
				const int nextSubsetStartIndex = (i + 1) * subsetSize;

				for (int j = endIndex - aSize - 1; j <= endIndex; ++j)
				{
					terrainVertices[nextSubsetStartIndex + (j - endIndex + aSize)].position = terrainVertices[j].
						position;
					terrainVertices[nextSubsetStartIndex + (j - endIndex + aSize)].normal = terrainVertices[j].normal;
					terrainVertices[nextSubsetStartIndex + (j - endIndex + aSize)].texCoord = terrainVertices[j].
						texCoord;
					terrainVertices[nextSubsetStartIndex + (j - endIndex + aSize)].tangent = terrainVertices[j].tangent;
					terrainVertices[nextSubsetStartIndex + (j - endIndex + aSize)].bitangent = terrainVertices[j].
						bitangent;
				}
			}

			terrainSubsets[i].vertices.assign(terrainVertices.begin() + startIndex, terrainVertices.begin() + endIndex);

			// Adjust the indices to be relative to the subset
			for (int z = 0; z < aSize - 1; ++z)
			{
				for (int x = 0; x < aSize - 1; ++x)
				{
					const int topLeftIndex = z * aSize + x;
					const int topRightIndex = topLeftIndex + 1;
					const int bottomLeftIndex = (z + 1) * aSize + x;
					const int bottomRightIndex = bottomLeftIndex + 1;

					// Check if all indices are within the subset range
					if (topLeftIndex >= startIndex && bottomRightIndex < endIndex)
					{
						// Adjust the indices relative to the subset
						terrainSubsets[i].indices.push_back(static_cast<unsigned short>(topLeftIndex - startIndex));
						terrainSubsets[i].indices.push_back(static_cast<unsigned short>(bottomLeftIndex - startIndex));
						terrainSubsets[i].indices.push_back(static_cast<unsigned short>(topRightIndex - startIndex));
						terrainSubsets[i].indices.push_back(static_cast<unsigned short>(topRightIndex - startIndex));
						terrainSubsets[i].indices.push_back(static_cast<unsigned short>(bottomLeftIndex - startIndex));
						terrainSubsets[i].indices.push_back(static_cast<unsigned short>(bottomRightIndex - startIndex));
					}
				}
			}
		}

		OutputDebugStringA("\nMoved to subsets...");

		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Grass\\Grass_c.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Grass\\Grass_n.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Grass\\Grass_m.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Rock\\Rock_c.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Rock\\Rock_n.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Rock\\Rock_m.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Snow\\Snow_c.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Snow\\Snow_n.png");
		texture.LoadTextureFromPath(aGfx, "Assets\\Textures\\Snow\\Snow_m.png");

		OutputDebugStringA("\nLoaded textures...");

		sampler.Init(aGfx, 0u);

		for (auto& subset : terrainSubsets)
		{
			subset.vertexBuffer.Init(aGfx, subset.vertices);
			subset.indexBuffer.Init(aGfx, subset.indices);
		}

		pixelShader.Init(aGfx, L"Shaders\\TerrainPhong_PS.cso");
		vertexShader.Init(aGfx, L"Shaders\\TerrainReflect_VS.cso");

		inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
		topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		rasterizer.Init(aGfx);
		depthStencil.Init(aGfx, DepthStencil::Mode::Write);

		OutputDebugStringA("\nDone!");
	}

	void Terrain::Draw(Graphics& aGfx)
	{
		for (auto& subset : terrainSubsets)
		{
			sampler.Bind(aGfx);
			texture.Bind(aGfx);

			subset.vertexBuffer.Bind(aGfx);
			subset.indexBuffer.Bind(aGfx);

			TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
			transformConstantBuffer.Bind(aGfx);

			pixelShader.Bind(aGfx);

			PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
			psConstantBuffer.Bind(aGfx);

			vertexShader.Bind(aGfx);
			inputLayout.Bind(aGfx);
			topology.Bind(aGfx);
			rasterizer.Bind(aGfx);
			depthStencil.Bind(aGfx);

			aGfx.DrawIndexed(static_cast<UINT>(std::size(subset.indices)));

			// Unbind shader resources
			ID3D11ShaderResourceView* nullSRVs[9] = {nullptr};
			aGfx.pContext->PSSetShaderResources(2u, 9u, nullSRVs);
		}
	}

	void Terrain::SetPosition(const DirectX::XMFLOAT3 aPosition)
	{
		transform.x = aPosition.x;
		transform.y = aPosition.y;
		transform.z = aPosition.z;
	}

	void Terrain::Move(const DirectX::XMFLOAT3 aDistance)
	{
		transform.x += aDistance.x;
		transform.y += aDistance.y;
		transform.z += aDistance.z;
	}

	void Terrain::FlipScale(const float aHeight, const bool aReset)
	{
		transform.scale.y *= -1.0f;
		if (aReset)
		{
			transform.y -= aHeight * 2.0f;
		}
		else
		{
			transform.y += aHeight * 2.0f;
		}
	}

	void Terrain::SetReflectShader(const Graphics& aGfx, const bool aValue)
	{
		if (aValue)
		{
			vertexShader.Init(aGfx, L"Shaders\\TerrainReflect_VS.cso");
		}
		else
		{
			vertexShader.Init(aGfx, L"Shaders\\TerrainPhong_VS.cso");
		}
	}

	void Terrain::SetCullingMode(const eCullingMode aMode)
	{
		rasterizer.SetCullingMode(aMode);
	}

	DirectX::XMFLOAT3 Terrain::GetRandomVertexPosition() const
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<> sDist(0, (int)terrainSubsets.size() - 1);
		const int subsetIndex = sDist(mt);
		std::uniform_int_distribution<> vDist(0, (int)terrainSubsets[subsetIndex].vertices.size() - 1);
		const int vertIndex = vDist(mt);

		float x = terrainSubsets[subsetIndex].vertices[vertIndex].position.x + transform.x;
		float y = terrainSubsets[subsetIndex].vertices[vertIndex].position.y + transform.y;
		float z = terrainSubsets[subsetIndex].vertices[vertIndex].position.z + transform.z;
		return {x, y, z};
	}

	DirectX::XMMATRIX Terrain::GetTransform() const
	{
		return DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z) *
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.roll) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	void Terrain::ShowControlWindow(const char* aWindowName)
	{
		aWindowName = aWindowName ? aWindowName : "Terrain";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::DragFloat3("XYZ", &transform.x);
			ImGui::Text("Scale");
			ImGui::DragFloat("XYZ", &transform.scale.x, 0.1f, 0.0f, 10.0f, "%.1f");
			ImGui::Text("Specular");
			ImGui::SliderFloat("Intensity", &pmc.specularIntensity, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
			ImGui::DragFloat("Power", &pmc.specularPower);
		}
		ImGui::End();
	}
}
