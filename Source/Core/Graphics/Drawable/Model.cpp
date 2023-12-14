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
		isLoaded = ModelLoader::LoadAnimatedModel(animatedModelData, aFilePath);

		if (isLoaded)
		{
			shaderType = aShaderType;
			texture.LoadTextureFromModel(aGfx, aFilePath);
		}

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
				modelData.skeleton.bones.emplace_back();
				auto& newBone = modelData.skeleton.bones.back();

				// Name
				newBone.name = bone.Name;

				// Matrix
				const auto& boneMatrix = bone.BindPoseInverse;
				newBone.bindPose = DirectX::XMMatrixSet(
					boneMatrix.Data[0], boneMatrix.Data[1], boneMatrix.Data[2], boneMatrix.Data[3],
					boneMatrix.Data[4], boneMatrix.Data[5], boneMatrix.Data[6], boneMatrix.Data[7],
					boneMatrix.Data[8], boneMatrix.Data[9], boneMatrix.Data[10], boneMatrix.Data[11],
					boneMatrix.Data[12], boneMatrix.Data[13], boneMatrix.Data[14], boneMatrix.Data[15]
				);

				// Transpose the matrix
				newBone.bindPose = DirectX::XMMatrixTranspose(newBone.bindPose);

				// Parent
				newBone.parentIndex = bone.Parent;

				// Add bone name
				modelData.skeleton.boneNames.push_back(bone.Name);

				// Add bone offset matrix to bind pose
				combinedTransforms.push_back(newBone.bindPose);
				finalTransforms.push_back(newBone.bindPose);
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

					vertices[v].boneIndices[0] = fbxMesh.Vertices[v].BoneIDs[0];
					vertices[v].boneIndices[1] = fbxMesh.Vertices[v].BoneIDs[1];
					vertices[v].boneIndices[2] = fbxMesh.Vertices[v].BoneIDs[2];
					vertices[v].boneIndices[3] = fbxMesh.Vertices[v].BoneIDs[3];

					vertices[v].boneWeights[0] = fbxMesh.Vertices[v].BoneWeights[0];
					vertices[v].boneWeights[1] = fbxMesh.Vertices[v].BoneWeights[1];
					vertices[v].boneWeights[2] = fbxMesh.Vertices[v].BoneWeights[2];
					vertices[v].boneWeights[3] = fbxMesh.Vertices[v].BoneWeights[3];
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

	bool Model::LoadFBXAnimation(const std::string& aFilePath)
	{
		return ModelLoader::LoadAnimation(animatedModelData, aFilePath);
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
					DirectX::XMMATRIX bones[64u];
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
						DirectX::XMMATRIX bones[64u];
					} vsb = {};

					for (int i = 0; i < finalTransforms.size(); ++i)
					{
						vsb.bones[i] = finalTransforms[i];
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

	void Model::DrawFBXPtr(Graphics& aGfx)
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

		for (AnimatedMesh& mesh : animatedModelData.meshList->meshes)
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
						DirectX::XMMATRIX bones[64u];
					} vsb = {};

					for (int i = 0; i < animatedModelData.finalTransforms.size(); ++i)
					{
						vsb.bones[i] = animatedModelData.finalTransforms[i];
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
		if (selectedAnimationIndex >= 0 && selectedAnimationIndex < modelData.animations.size())
		{
			const AnimationClip& animation = modelData.animations[selectedAnimationIndex];

			// Apply animation transformation based on the current time
			if (isAnimationPlaying)
			{
				animationTime += aDeltaTime * animationSpeed;

				if (isAnimationLooping)
				{
					if (animationTime >= animation.duration)
					{
						animationTime = 0.0f;
					}
				}
				else
				{
					if (animationTime >= animation.duration)
					{
						animationTime = animation.duration;
						return;
					}
				}

				// Calculate the current frame and delta
				const float frameRate = 1.0f / animation.fps;
				const float result = animationTime / frameRate;
				const size_t frame = std::floor(result); // Current frame
				const float delta = result - static_cast<float>(frame); // Progress to the next frame

				// Update all animations
				const Skeleton* skeleton = &modelData.skeleton;

				// Interpolate between current and next frame
				for (size_t i = 0; i < skeleton->bones.size(); i++)
				{
					DirectX::XMMATRIX currentFramePose = animation.keyframes[frame].boneTransforms[i];
					DirectX::XMMATRIX nextFramePose = animation.keyframes[(frame + 1) % animation.keyframes.size()].
						boneTransforms[i];

					// Interpolate between current and next frame using delta
					DirectX::XMMATRIX blendedPose = currentFramePose + delta * (nextFramePose - currentFramePose);

					const int parentIndex = skeleton->bones[i].parentIndex;

					if (parentIndex >= 0)
					{
						// Accumulate relative transformation
						combinedTransforms[i] = blendedPose * combinedTransforms[parentIndex];
					}
					else
					{
						// Root bone, use absolute transformation
						combinedTransforms[i] = blendedPose;
					}

					finalTransforms[i] = skeleton->bones[i].bindPose * combinedTransforms[i];
				}
			}
			else
			{
				// Play paused frame i.e do nothing
			}
		}
		else
		{
			animationTime = 0.0f;

			// Show bind pose (T-pose)
			const Skeleton* skeleton = &modelData.skeleton;

			for (size_t i = 0; i < skeleton->bones.size(); i++)
			{
				finalTransforms[i] = DirectX::XMMatrixIdentity();
			}
		}
	}

	void Model::UpdatePtr(float aDeltaTime)
	{
		if (selectedAnimationIndex >= 0 && selectedAnimationIndex < animatedModelData.animationNames.size())
		{
			std::string selectedAnimationName = animatedModelData.animationNames[selectedAnimationIndex];
			AnimationClip* animation = animatedModelData.animationClipMap[selectedAnimationName];

			// Apply animation transformation based on the current time
			if (isAnimationPlaying)
			{
				animationTime += aDeltaTime * animationSpeed;

				if (isAnimationLooping)
				{
					if (animationTime >= animation->duration)
					{
						animationTime = 0.0f;
					}
				}
				else
				{
					if (animationTime >= animation->duration)
					{
						animationTime = animation->duration;
						return;
					}
				}

				// Calculate the current frame and delta
				const float frameRate = 1.0f / animation->fps;
				const float result = animationTime / frameRate;
				const size_t frame = std::floor(result); // Current frame
				const float delta = result - static_cast<float>(frame); // Progress to the next frame

				// Update all animations
				const Skeleton* skeleton = animatedModelData.skeleton;

				// Interpolate between current and next frame
				for (size_t i = 0; i < skeleton->bones.size(); i++)
				{
					DirectX::XMMATRIX currentFramePose = animation->keyframes[frame].boneTransforms[i];
					DirectX::XMMATRIX nextFramePose = animation->keyframes[(frame + 1) % animation->keyframes.size()].
						boneTransforms[i];

					// Interpolate between current and next frame using delta
					DirectX::XMMATRIX blendedPose = currentFramePose + delta * (nextFramePose - currentFramePose);

					const int parentIndex = skeleton->bones[i].parentIndex;

					if (parentIndex >= 0)
					{
						// Accumulate relative transformation
						animatedModelData.combinedTransforms[i] = blendedPose * animatedModelData.combinedTransforms[parentIndex];
					}
					else
					{
						// Root bone, use absolute transformation
						animatedModelData.combinedTransforms[i] = blendedPose;
					}

					animatedModelData.finalTransforms[i] = skeleton->bones[i].bindPose * animatedModelData.combinedTransforms[i];
				}
			}
			else
			{
				// Play paused frame i.e do nothing
			}
		}
		else
		{
			animationTime = 0.0f;

			// Show bind pose (T-pose)
			const Skeleton* skeleton = animatedModelData.skeleton;

			for (size_t i = 0; i < skeleton->bones.size(); i++)
			{
				animatedModelData.finalTransforms[i] = DirectX::XMMatrixIdentity();
			}
		}
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
				                      ? animatedModelData.animationNames[selectedAnimationIndex].c_str()
				                      : "Select Animation"))
			{
				for (int i = 0; i < animatedModelData.animationNames.size(); ++i)
				{
					const bool isSelected = (selectedAnimationIndex == i);
					if (ImGui::Selectable(animatedModelData.animationNames[i].c_str(), isSelected))
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
			if (ImGui::Checkbox("Loop", &isAnimationLooping)) { }

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
