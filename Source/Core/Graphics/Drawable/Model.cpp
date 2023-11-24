#include "stdafx.h"
#include "Model.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Drawable/ModelLoader.h"
#include <External/include/imgui/imgui.h>
#include <d3dcompiler.h>
#include <TGAFBXImporter/source/Internal.inl>
#include <wrl/client.h>

#include "TGAFBXImporter/source/FBXImporter.h"

namespace Kaka
{
	static constexpr UINT MAX_LIGHTS = 50u;

	Model::Model(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType)
		:
		shaderType(aShaderType)
	{
		isLoaded = ModelLoader::LoadModel(aFilePath, modelData);
		texture.LoadTextureFromModel(aGfx, aFilePath);
		solidColour = {1.0f, 1.0f, 1.0f, 1.0f};
	}

	void Model::LoadModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType)
	{
		if (!isLoaded)
		{
			shaderType = aShaderType;
			isLoaded = ModelLoader::LoadModel(aFilePath, modelData);
			texture.LoadTextureFromModel(aGfx, aFilePath);
			solidColour = {1.0f, 1.0f, 1.0f, 1.0f};
		}

		if (isLoaded)
		{
			sampler.Init(aGfx, 0u);

			// Vertices and indices
			switch (modelData.modelType)
			{
				case eModelType::None:
				{
					assert("No model type!");
				}
				break;
				case eModelType::Static:
				{
					vertexBuffer.Init(aGfx, modelData.mesh.vertices);
					indexBuffer.Init(aGfx, modelData.mesh.indices);
				}
				break;
				case eModelType::Skeletal:
				{
					vertexBuffer.Init(aGfx, modelData.animMesh.vertices);
					indexBuffer.Init(aGfx, modelData.animMesh.indices);
				}
				break;
				default:
					assert("Error!");
			}

			// Shaders
			switch (shaderType)
			{
				case eShaderType::Solid:
				{
					pixelShader.Init(aGfx, L"Shaders\\Solid_PS.cso");
					vertexShader.Init(aGfx, L"Shaders\\Solid_VS.cso");

					ied =
					{
						{
							"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
					};
				}
				break;
				case eShaderType::Light:
				{
					pixelShader.Init(aGfx, L"Shaders\\Light_PS.cso");
					vertexShader.Init(aGfx, L"Shaders\\Light_VS.cso");

					ied =
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
						{
							"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
					};
				}
				break;
				case eShaderType::Phong:
				{
					pixelShader.Init(aGfx, L"Shaders\\Phong_PS.cso");
					vertexShader.Init(aGfx, L"Shaders\\Phong_VS.cso");

					ied =
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
						{
							"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
					};
					inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
				}
				break;
				case eShaderType::AnimPhong:
				{
					pixelShader.Init(aGfx, L"Shaders\\Phong_PS.cso");
					vertexShader.Init(aGfx, L"Shaders\\AnimPhong_VS.cso");

					ied =
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
						{
							"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
					};
				}
				case eShaderType::PBR:
				{
					pixelShader.Init(aGfx, L"Shaders\\ModelPBR_PS.cso");
					vertexShader.Init(aGfx, L"Shaders\\ModelPBR_VS.cso");

					ied =
					{
						{
							"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
					};
					inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
				}
				break;
			}

			inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
			topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			rasterizer.Init(aGfx);
			depthStencil.Init(aGfx, DepthStencil::Mode::Write);
		}
	}

	void Model::LoadFBXModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType)
	{
		if (!isLoaded)
		{
			shaderType = aShaderType;
			TGA::FBXModel fbxModel;
			isLoaded = TGA::FBXImporter::LoadModel(aFilePath, fbxModel);
			texture.LoadTextureFromModel(aGfx, aFilePath);

			modelData.modelType = eModelType::Skeletal;

			// Copy bone data from FBXImporter to our own model data
			for (auto& bone : fbxModel.Skeleton.Bones)
			{
				if (bone.Name == "character___rig:r_toe_jnt")
				{
					int a = 0;
				}
				modelData.skeleton.bones.emplace_back();
				auto& newBone = modelData.skeleton.bones.back();

				// Name
				newBone.name = bone.Name;

				// Matrix
				const auto& boneMatrix = bone.BindPoseInverse;
				newBone.bindPose._11 = boneMatrix.Data[0];
				newBone.bindPose._12 = boneMatrix.Data[1];
				newBone.bindPose._13 = boneMatrix.Data[2];
				newBone.bindPose._14 = boneMatrix.Data[3];
				newBone.bindPose._21 = boneMatrix.Data[4];
				newBone.bindPose._22 = boneMatrix.Data[5];
				newBone.bindPose._23 = boneMatrix.Data[6];
				newBone.bindPose._24 = boneMatrix.Data[7];
				newBone.bindPose._31 = boneMatrix.Data[8];
				newBone.bindPose._32 = boneMatrix.Data[9];
				newBone.bindPose._33 = boneMatrix.Data[10];
				newBone.bindPose._34 = boneMatrix.Data[11];
				newBone.bindPose._41 = boneMatrix.Data[12];
				newBone.bindPose._42 = boneMatrix.Data[13];
				newBone.bindPose._43 = boneMatrix.Data[14];
				newBone.bindPose._44 = boneMatrix.Data[15];

				// Transpose the matrix
				DirectX::XMStoreFloat4x4(&newBone.bindPose,
				                         DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&newBone.bindPose)));

				// Parent
				newBone.parentIndex = bone.Parent;

				// Children
				//for (const auto& child : bone.Children)
				//{
				//	newBone.childIndices.push_back((int)child);
				//}

				// Add bone to the bone index map
				//modelData.skeleton.boneIndexMap.insert({bone.Name, (int)modelData.skeleton.bones.size() - 1});

				// Add bone name
				modelData.skeleton.boneNames.push_back(bone.Name);

				// Add bone offset matrix to bind pose
				//modelData.bindPose.push_back(newBone.offsetMatrix);
				combinedTransforms.push_back(newBone.bindPose);
				finalTransform.push_back(newBone.bindPose);
			}

			// Copy model data from FBXImporter to our own model data
			modelData.animMeshes.resize(fbxModel.Meshes.size());
			for (size_t i = 0; i < modelData.animMeshes.size(); ++i)
			{
				// Imported data
				TGA::FBXModel::FBXMesh& fbxMesh = fbxModel.Meshes[i];

				// Our own data
				AnimatedMesh& mesh = modelData.animMeshes[i];

				std::vector<BoneVertex> vertices;
				vertices.resize(fbxMesh.Vertices.size());

				// Copy vertex data
				for (size_t v = 0; v < vertices.size(); ++v)
				{
					vertices[v].position = {
						fbxMesh.Vertices[v].Position[0],
						fbxMesh.Vertices[v].Position[1],
						fbxMesh.Vertices[v].Position[2]
					};

					vertices[v].normal = {
						fbxMesh.Vertices[v].Normal[0],
						fbxMesh.Vertices[v].Normal[1],
						fbxMesh.Vertices[v].Normal[2]
					};

					vertices[v].tangent = {
						fbxMesh.Vertices[v].Tangent[0],
						fbxMesh.Vertices[v].Tangent[1],
						fbxMesh.Vertices[v].Tangent[2]
					};

					vertices[v].bitangent = {
						fbxMesh.Vertices[v].Binormal[0],
						fbxMesh.Vertices[v].Binormal[1],
						fbxMesh.Vertices[v].Binormal[2]
					};

					// TODO This may be a problem, four UV channels?
					vertices[v].texCoord.x = fbxMesh.Vertices[v].UVs[0][0];
					vertices[v].texCoord.y = fbxMesh.Vertices[v].UVs[0][1];

					std::string texCoords = "\nTexture coords: " + std::to_string(vertices[v].texCoord.x) + ", " +
						std::to_string(vertices[v].texCoord.y);
					OutputDebugStringA(texCoords.c_str());

					vertices[v].boneIndices[0] = fbxMesh.Vertices[v].BoneIDs[0];
					vertices[v].boneIndices[1] = fbxMesh.Vertices[v].BoneIDs[1];
					vertices[v].boneIndices[2] = fbxMesh.Vertices[v].BoneIDs[2];
					vertices[v].boneIndices[3] = fbxMesh.Vertices[v].BoneIDs[3];

					vertices[v].boneWeights[0] = fbxMesh.Vertices[v].BoneWeights[0];
					vertices[v].boneWeights[1] = fbxMesh.Vertices[v].BoneWeights[1];
					vertices[v].boneWeights[2] = fbxMesh.Vertices[v].BoneWeights[2];
					vertices[v].boneWeights[3] = fbxMesh.Vertices[v].BoneWeights[3];

					//// Normalize bone weights
					//float weightSum = vertices[v].boneWeights[0] + vertices[v].boneWeights[1] +
					//	vertices[v].boneWeights[2] + vertices[v].boneWeights[3];
					//for (int w = 0; w < 4; ++w)
					//{
					//	vertices[v].boneWeights[w] /= weightSum;
					//}
				}

				mesh.vertices = vertices;

				for (const auto& index : fbxMesh.Indices)
				{
					mesh.indices.push_back(index);
				}
			}

			//vertexBuffer.Init(aGfx, modelData.animMesh.vertices);
			//indexBuffer.Init(aGfx, modelData.animMesh.indices);

			texture.LoadTextureFromModel(aGfx, aFilePath);
			solidColour = {1.0f, 1.0f, 1.0f, 1.0f};
		}

		if (isLoaded)
		{
			sampler.Init(aGfx, 0u);

			// Vertices and indices
			switch (modelData.modelType)
			{
				case eModelType::None:
				{
					assert("No model type!");
				}
				break;
				case eModelType::Static:
				{
					vertexBuffer.Init(aGfx, modelData.mesh.vertices);
					indexBuffer.Init(aGfx, modelData.mesh.indices);
				}
				break;
				case eModelType::Skeletal:
				{
					//vertexBuffer.Init(aGfx, modelData.animMesh.vertices);
					//indexBuffer.Init(aGfx, modelData.animMesh.indices);
				}
				break;
				default:
					assert("Error!");
			}

			// Shaders
			switch (shaderType)
			{
				case eShaderType::AnimPBR:
				{
					pixelShader.Init(aGfx, L"Shaders\\ModelPBR_PS.cso");
					vertexShader.Init(aGfx, L"Shaders\\AnimModelPBR_VS.cso");

					ied =
					{
						{
							"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
						{
							"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
							D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
						},
					};
					inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
				}
				break;
			}

			inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
			topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			rasterizer.Init(aGfx);
			depthStencil.Init(aGfx, DepthStencil::Mode::Write);
		}
	}

	void Model::Draw(Graphics& aGfx)
	{
		//if (!aGfx.IsBoundingBoxInFrustum(GetPosition(), GetPosition()))
		//{
		//	return;
		//}

		if (!isLoaded)
		{
			return;
		}

		sampler.Bind(aGfx);
		texture.Bind(aGfx);

		vertexBuffer.Bind(aGfx);
		indexBuffer.Bind(aGfx);

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);
		pixelShader.Bind(aGfx);

		switch (shaderType)
		{
			case eShaderType::Solid:
			{
				struct PSMaterialConstant
				{
					DirectX::XMFLOAT4 colour;
				} pmc = {};
				pmc.colour = solidColour;

				PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
				psConstantBuffer.Bind(aGfx);
			}
			break;
			case eShaderType::Light:
			{
				struct PSMaterialConstant
				{
					BOOL normalMapEnabled = FALSE;
					BOOL materialEnabled = FALSE;
					BOOL padding1 = {};
					BOOL padding2 = {};
				} pmc;
				pmc.normalMapEnabled = texture.HasNormalMap();
				pmc.materialEnabled = texture.HasMaterial();

				PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
				psConstantBuffer.Bind(aGfx);
			}
			break;
			case eShaderType::Phong:
			{
				struct PSMaterialConstant
				{
					BOOL normalMapEnabled = FALSE;
					BOOL materialEnabled = FALSE;
					float specularIntensity = 0.1f;
					float specularPower = 30.0f;
				} pmc;
				pmc.normalMapEnabled = texture.HasNormalMap();
				pmc.materialEnabled = texture.HasMaterial();
				pmc.specularIntensity = specularIntensity;
				pmc.specularPower = specularPower;

				PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
				psConstantBuffer.Bind(aGfx);
			}
			break;
			case eShaderType::AnimPhong:
			{
				struct PSMaterialConstant
				{
					BOOL normalMapEnabled = FALSE;
					BOOL materialEnabled = FALSE;
					float specularIntensity = 0.1f;
					float specularPower = 30.0f;
				} pmc;
				pmc.normalMapEnabled = texture.HasNormalMap();
				pmc.materialEnabled = texture.HasMaterial();
				pmc.specularIntensity = specularIntensity;
				pmc.specularPower = specularPower;

				PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
				psConstantBuffer.Bind(aGfx);

				struct VSBoneConstant
				{
					DirectX::XMFLOAT4X4 bones[64u];
				} vsb = {};

				for (int i = 0; i < modelData.skeleton.bones.size(); ++i)
				{
					vsb.bones[i] = modelData.skeleton.bones[i].bindPose;
				}

				VertexConstantBuffer<VSBoneConstant> vsConstantBuffer(aGfx, vsb, 1u);
				vsConstantBuffer.Bind(aGfx);
			}
			break;
			case eShaderType::PBR:
			{
				struct PSMaterialConstant
				{
					BOOL normalMapEnabled = FALSE;
					BOOL materialEnabled = FALSE;
					unsigned int packedNearbyPointLightDataA = 0u;
					unsigned int packedNearbyPointLightDataB = 0u;
					unsigned int packedNearbySpotLightDataA = 0u;
					unsigned int packedNearbySpotLightDataB = 0u;
					float padding[2];
				} pmc;
				pmc.normalMapEnabled = texture.HasNormalMap();
				pmc.materialEnabled = texture.HasMaterial();
				for (int i = 0; i < MAX_LIGHTS; ++i)
				{
					if (i < 32)
					{
						pmc.packedNearbyPointLightDataA |= (nearbyPointLights[i] ? (1u << i) : 0u);
						pmc.packedNearbySpotLightDataA |= (nearbySpotLights[i] ? (1u << i) : 0u);
					}
					else
					{
						pmc.packedNearbyPointLightDataA |= (nearbyPointLights[i - 32] ? (1u << (i - 32)) : 0u);
						pmc.packedNearbySpotLightDataA |= (nearbySpotLights[i - 32] ? (1u << (i - 32)) : 0u);
					}
				}

				PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
				psConstantBuffer.Bind(aGfx);
			}
		}

		vertexShader.Bind(aGfx);
		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);
		rasterizer.Bind(aGfx);
		depthStencil.Bind(aGfx);

		std::vector<unsigned short> indices;

		switch (modelData.modelType)
		{
			case eModelType::Static:
				indices = modelData.mesh.indices;
				break;
			case eModelType::Skeletal:
				indices = modelData.animMesh.indices;
				break;
		}

		aGfx.DrawIndexed(static_cast<UINT>(std::size(indices)));
		//aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[3] = {nullptr};
		aGfx.pContext->PSSetShaderResources(0u, 3u, nullSRVs);
		//}
	}

	void Model::DrawFBX(Graphics& aGfx)
	{
		if (!isLoaded)
		{
			return;
		}

		sampler.Bind(aGfx);
		texture.Bind(aGfx);

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		vertexShader.Bind(aGfx);
		pixelShader.Bind(aGfx);
		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);
		rasterizer.Bind(aGfx);
		depthStencil.Bind(aGfx);

		for (auto& mesh : modelData.animMeshes)
		{
			vertexBuffer.Init(aGfx, mesh.vertices);
			indexBuffer.Init(aGfx, mesh.indices);

			vertexBuffer.Bind(aGfx);
			indexBuffer.Bind(aGfx);

			switch (shaderType)
			{
				case eShaderType::AnimPBR:
				{
					struct PSMaterialConstant
					{
						BOOL normalMapEnabled = FALSE;
						BOOL materialEnabled = FALSE;
						unsigned int packedNearbyPointLightDataA = 0u;
						unsigned int packedNearbyPointLightDataB = 0u;
						unsigned int packedNearbySpotLightDataA = 0u;
						unsigned int packedNearbySpotLightDataB = 0u;
						float padding[2];
					} pmc;
					pmc.normalMapEnabled = texture.HasNormalMap();
					pmc.materialEnabled = texture.HasMaterial();
					for (int i = 0; i < MAX_LIGHTS; ++i)
					{
						if (i < 32)
						{
							pmc.packedNearbyPointLightDataA |= (nearbyPointLights[i] ? (1u << i) : 0u);
							pmc.packedNearbySpotLightDataA |= (nearbySpotLights[i] ? (1u << i) : 0u);
						}
						else
						{
							pmc.packedNearbyPointLightDataA |= (nearbyPointLights[i - 32] ? (1u << (i - 32)) : 0u);
							pmc.packedNearbySpotLightDataA |= (nearbySpotLights[i - 32] ? (1u << (i - 32)) : 0u);
						}
					}

					PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
					psConstantBuffer.Bind(aGfx);

					// Bones
					struct VSBoneConstant
					{
						DirectX::XMFLOAT4X4 bones[64u];
					} vsb = {};

					for (int i = 0; i < finalTransform.size(); ++i)
					{
						vsb.bones[i] = finalTransform[i];
					}

					VertexConstantBuffer<VSBoneConstant> vsConstantBuffer(aGfx, vsb, 1u);
					vsConstantBuffer.Bind(aGfx);
				}
				break;
			}

			std::vector<unsigned short> indices;
			indices = mesh.indices;

			aGfx.DrawIndexed(static_cast<UINT>(std::size(indices)));
		}
		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[3] = {nullptr};
		aGfx.pContext->PSSetShaderResources(0u, 3u, nullSRVs);
	}

	void Model::Update(const float aDeltaTime)
	{
		animationTime += aDeltaTime;

		if (animationTime >= modelData.animations[0].duration)
		{
			//if (true)
			{
				while (animationTime >= modelData.animations[0].duration)
				{
					animationTime -= modelData.animations[0].duration;
				}
			}
			//else
			//{
			//	animationTime = GetAnimation()->Duration;
			//	myState = AnimationState::Finished;
			//}
		}

		const float frameRate = 1.0f / modelData.animations[0].fps;
		const float result = animationTime / frameRate;
		const size_t frame = static_cast<size_t>(std::floor(result)); // Which frame we're on
		const float delta = result - static_cast<float>(frame); // How far we have progressed to the next frame.

		// Update all animations
		Skeleton* skeleton = &modelData.skeleton;

		std::string text = "\n" + std::to_string(frame);
		OutputDebugStringA(text.c_str());

		// Interpolate between current and next frame
		for (size_t i = 0; i < skeleton->bones.size(); i++)
		{
			//DirectX::XMMATRIX bindPose = DirectX::XMLoadFloat4x4(&modelData.bindPose[i]);
			DirectX::XMMATRIX currentFramePose = DirectX::XMLoadFloat4x4(
				&modelData.animations[0].keyframes[frame].boneTransforms[i]);
			DirectX::XMMATRIX nextFramePose = DirectX::XMLoadFloat4x4(
				&modelData.animations[0].keyframes[(frame + 1) % modelData.animations[0].keyframes.size()].boneTransforms[i]);

			DirectX::XMMATRIX blendedPose = currentFramePose + delta * (nextFramePose - currentFramePose);

			int parentIndex = skeleton->bones[i].parentIndex;

			if (parentIndex >= 0)
			{
				// Accumulate relative transformation
				DirectX::XMStoreFloat4x4(&combinedTransforms[i],
				                         DirectX::XMMatrixMultiply(blendedPose,
				                                                   DirectX::XMLoadFloat4x4(
					                                                   &combinedTransforms[parentIndex])));
			}
			else
			{
				DirectX::XMStoreFloat4x4(&combinedTransforms[i], blendedPose);
			}

			DirectX::XMStoreFloat4x4(&finalTransform[i],
			                         DirectX::XMMatrixMultiply(
				                         DirectX::XMLoadFloat4x4(&skeleton->bones[i].bindPose),
				                         DirectX::XMLoadFloat4x4(&combinedTransforms[i])));
		}
	}

	//void Model::TraverseBoneHierarchy(int aBoneIndex, const DirectX::XMMATRIX& aParentTransform,
	//                                  std::vector<DirectX::XMFLOAT4X4>& aInterpolatedBoneTransforms,
	//                                  const Keyframe& aKeyframe1, const Keyframe& aKeyframe2, const float aT)
	//{
	//	if (aBoneIndex < 0)
	//	{
	//		aBoneIndex++;
	//	}
	//	// Calculate the interpolated transformation for the current bone using keyframe data
	//	const DirectX::XMFLOAT4X4& transform1 = aKeyframe1.boneTransforms[aBoneIndex];
	//	const DirectX::XMFLOAT4X4& transform2 = aKeyframe2.boneTransforms[aBoneIndex];

	//	DirectX::XMFLOAT4X4 interpolatedTransform{};
	//	for (int row = 0; row < 4; ++row)
	//	{
	//		for (int col = 0; col < 4; ++col)
	//		{
	//			interpolatedTransform.m[row][col] = transform1.m[row][col] + aT * (transform2.m[row][col] - transform1.m
	//				[row][col]);
	//		}
	//	}

	//	// Calculate the final bone transform by multiplying with the parent's transform
	//	const DirectX::XMMATRIX boneTransform = DirectX::XMLoadFloat4x4(&modelData.globalInverseMatrix) *
	//		DirectX::XMLoadFloat4x4(&interpolatedTransform) * aParentTransform;

	//	// Store the bone transform in the interpolatedBoneTransforms array
	//	DirectX::XMStoreFloat4x4(&aInterpolatedBoneTransforms[aBoneIndex], boneTransform);

	//	const Bone& bone = modelData.skeleton.bones[aBoneIndex];
	//	for (const int childIndex : bone.childIndices)
	//	{
	//		TraverseBoneHierarchy(childIndex, boneTransform, aInterpolatedBoneTransforms, aKeyframe1, aKeyframe2, aT);
	//	}
	//}

	//void Model::Animate()
	//{
	//	if (modelData.modelType != eModelType::Skeletal)
	//	{
	//		return;
	//	}
	//	if (selectedAnimationIndex >= 0 && selectedAnimationIndex < modelData.animations.size())
	//	{
	//		const AnimationClip& animation = modelData.animations[selectedAnimationIndex];

	//		// Apply animation transformation based on the current time
	//		if (isAnimationPlaying)
	//		{
	//			// Find the two keyframes for interpolation
	//			size_t keyframeIndex1 = 0;
	//			size_t keyframeIndex2 = 0;
	//			for (size_t i = 0; i < animation.keyframes.size() - 1; ++i)
	//			{
	//				if (animation.keyframes[i + 1].time > animationTime)
	//				{
	//					keyframeIndex1 = i;
	//					keyframeIndex2 = i + 1;
	//					break;
	//				}
	//			}

	//			// Interpolate between the two keyframes
	//			const Keyframe& keyframe1 = animation.keyframes[keyframeIndex1];
	//			const Keyframe& keyframe2 = animation.keyframes[keyframeIndex2];
	//			float t = std::clamp((animationTime - keyframe1.time) / (keyframe2.time - keyframe1.time), 0.0f, 1.0f);

	//			// Calculate the bone transforms for each keyframe bone
	//			std::vector<DirectX::XMFLOAT4X4> interpolatedBoneTransforms(modelData.skeleton.bones.size());

	//			// Traverse the bone hierarchy and calculate the interpolated bone transformations
	//			TraverseBoneHierarchy(modelData.skeleton.rootBoneIndex, DirectX::XMMatrixIdentity(),
	//			                      interpolatedBoneTransforms, keyframe1, keyframe2, t);

	//			// Update the bone transformations in the skeleton
	//			for (size_t i = 0; i < modelData.skeleton.bones.size(); ++i)
	//			{
	//				DirectX::XMFLOAT4X4& boneTransform = modelData.skeleton.bones[i].offsetMatrix;
	//				DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMLoadFloat4x4(&interpolatedBoneTransforms[i]));
	//			}
	//		}
	//		else
	//		{
	//			// Play paused frame
	//		}
	//	}
	//	else
	//	{
	//		// Update the bone transformations in the skeleton
	//		for (size_t i = 0; i < modelData.skeleton.bones.size(); ++i)
	//		{
	//			DirectX::XMFLOAT4X4& bindPoseTransform = modelData.bindPose[i];
	//			DirectX::XMFLOAT4X4& boneTransform = modelData.skeleton.bones[i].offsetMatrix;
	//			DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMLoadFloat4x4(&bindPoseTransform));
	//		}
	//	}
	//}

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

	void Model::SetColour(const DirectX::XMFLOAT4 aColour)
	{
		solidColour = aColour;
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

	bool Model::IsLoaded() const
	{
		return isLoaded;
	}

	void Model::SetNearbyLights(bool aNearbyPointLights[50], bool aNearbySpotLights[50])
	{
		for (int i = 0; i < MAX_LIGHTS; ++i)
		{
			nearbyPointLights[i] = aNearbyPointLights[i];
			nearbySpotLights[i] = aNearbySpotLights[i];
		}
	}

	void Model::BindPixelShader(const Graphics& aGfx)
	{
		pixelShader.Bind(aGfx);
	}

	void Model::ShowControlWindow(const char* aWindowName)
	{
		// Window name defaults to "Model"
		aWindowName = aWindowName ? aWindowName : "Model";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Columns(2, nullptr, true);
			// Dropdown animations
			// Select and press button to play
			// Change play speed

			// Dropdown animations
			ImGui::Text("Animations");
			if (ImGui::BeginCombo("##Animations",
			                      selectedAnimationIndex >= 0
				                      ? modelData.animations[selectedAnimationIndex].name.c_str()
				                      : "Select Animation"))
			{
				for (int i = 0; i < modelData.animations.size(); ++i)
				{
					const bool isSelected = (selectedAnimationIndex == i);
					if (ImGui::Selectable(modelData.animations[i].name.c_str(), isSelected))
					{
						selectedAnimationIndex = i;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			// Play button
			if (selectedAnimationIndex >= 0)
			{
				ImGui::Text("Play");
				if (isAnimationPlaying)
				{
					if (ImGui::Button("Pause"))
					{
						isAnimationPlaying = false;
					}
				}
				else
				{
					if (ImGui::Button("Play"))
					{
						isAnimationPlaying = true;
					}
				}
				if (ImGui::Button("Stop"))
				{
					isAnimationPlaying = false;
					selectedAnimationIndex = -1;
				}
			}

			// Animation speed slider
			ImGui::Text("Speed");
			ImGui::SliderFloat("##Speed", &animationSpeed, 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);

			ImGui::NextColumn();
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::DragFloat3("XYZ", &transform.x);
			ImGui::Text("Scale");
			ImGui::DragFloat("XYZ", &transform.scale, 0.1f, 0.0f, 10.0f, "%.1f");
			ImGui::Text("Specular");
			ImGui::SliderFloat("Intensity", &specularIntensity, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
			ImGui::DragFloat("Power", &specularPower);
		}
		ImGui::End();
	}
}
