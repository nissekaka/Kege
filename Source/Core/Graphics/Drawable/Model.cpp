#include "stdafx.h"
#include "Model.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Drawable/ModelLoader.h"
#include <External/include/imgui/imgui.h>
#include <d3dcompiler.h>
#include <TGAFBXImporter/source/Internal.inl>
#include <wrl/client.h>

#include "Graphics/Shaders/ShaderFactory.h"
#include "TGAFBXImporter/source/FBXImporter.h"

namespace Kaka
{
	static constexpr UINT MAX_LIGHTS = 50u;

	Model::Model(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType)
		:
		shaderType(aShaderType) { }

	void Model::Init()
	{
		if (modelType == eModelType::Skeletal)
		{
			animationPlayer.Init(&animatedModelData);
		}
		isLoaded = true;
	}

	void Model::LoadModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType)
	{
		shaderType = aShaderType;

		//sampler.Init(aGfx, 0u);

		// Shaders
		switch (shaderType)
		{
		case eShaderType::PBR:
			{
				modelType = eModelType::Static;
				ModelLoader::LoadStaticModel(aGfx, aFilePath, modelData);

				for (Mesh& mesh : modelData.meshList->meshes)
				{
					mesh.vertexBuffer.Init(aGfx, mesh.vertices);
					mesh.indexBuffer.Init(aGfx, mesh.indices);
				}
				//ModelLoader::LoadStaticModel(aGfx, aFilePath, modelData);
				//ModelLoader::LoadModel(modelData, aFilePath);
				//ModelLoader::LoadTexture(aGfx, modelData, aFilePath);

				pixelShader = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\ModelPBR_Deferred_PS.cso");
				vertexShader = ShaderFactory::GetVertexShader(aGfx, L"Shaders\\ModelPBR_Deferred_VS.cso");

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
				inputLayout.Init(aGfx, ied, vertexShader->GetBytecode());
			}
			break;
		case eShaderType::AnimPBR:
			{
				modelType = eModelType::Skeletal;

				ModelLoader::LoadAnimatedModel(animatedModelData, aFilePath);
				ModelLoader::LoadTexture(aGfx, animatedModelData, aFilePath);

				pixelShader = ShaderFactory::GetPixelShader(aGfx, L"Shaders\\ModelPBR_PS.cso");
				vertexShader = ShaderFactory::GetVertexShader(aGfx, L"Shaders\\AnimModelPBR_VS.cso");

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
				inputLayout.Init(aGfx, ied, vertexShader->GetBytecode());
			}
			break;
		}

		inputLayout.Init(aGfx, ied, vertexShader->GetBytecode());
		topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//rasterizer.Init(aGfx);
		//depthStencil.Init(aGfx, DepthStencil::Mode::Write);
	}

	bool Model::LoadAnimation(const std::string& aFilePath)
	{
		return ModelLoader::LoadAnimation(animatedModelData, aFilePath);
	}

	void Model::Draw(Graphics& aGfx, const float aDeltaTime)
	{
		switch (modelType)
		{
		case eModelType::Static:
			{
				DrawStatic(aGfx);
			}
			break;
		case eModelType::Skeletal:
			{
				UpdatePtr(aDeltaTime);
				DrawAnimated(aGfx);
			}
			break;
		default: ;
		}
	}

	void Model::DrawStatic(Graphics& aGfx)
	{
		if (!isLoaded)
		{
			return;
		}

		//sampler.Bind(aGfx);
		//modelData.texture->Bind(aGfx);

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		vertexShader->Bind(aGfx);
		if (aGfx.HasPixelShaderOverride())
		{
			aGfx.GetPixelShaderOverride()->Bind(aGfx);
		}
		else
		{
			pixelShader->Bind(aGfx);
		}
		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);
		//rasterizer.Bind(aGfx);
		//depthStencil.Bind(aGfx);

		for (Mesh& mesh : modelData.meshList->meshes)
		{
			if (mesh.texture != nullptr)
			{
				mesh.texture->Bind(aGfx);
			}

			mesh.vertexBuffer.Bind(aGfx);
			mesh.indexBuffer.Bind(aGfx);

			std::vector<unsigned short> indices;
			indices = mesh.indices;

			aGfx.DrawIndexed(static_cast<UINT>(std::size(indices)));
		}

		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[3] = {nullptr};
		aGfx.pContext->PSSetShaderResources(0u, 3u, nullSRVs);
	}

	void Model::DrawAnimated(Graphics& aGfx)
	{
		if (!isLoaded)
		{
			return;
		}

		//sampler.Bind(aGfx);
		animatedModelData.texture->Bind(aGfx);

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		vertexShader->Bind(aGfx);
		if (aGfx.HasPixelShaderOverride())
		{
			aGfx.GetPixelShaderOverride()->Bind(aGfx);
		}
		else
		{
			pixelShader->Bind(aGfx);
		}
		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);
		//rasterizer.Bind(aGfx);
		//depthStencil.Bind(aGfx);

		for (AnimatedMesh& mesh : animatedModelData.meshList->meshes)
		{
			mesh.vertexBuffer.Bind(aGfx);
			mesh.indexBuffer.Bind(aGfx);

			switch (shaderType)
			{
			case eShaderType::AnimPBR:
				{
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

		if (!drawSkeleton)
		{
			return;
		}
		if (aGfx.HasPixelShaderOverride())
		{
			return;
		}
		// Draw ImGui lines between bones
		const std::vector<DirectX::XMMATRIX> transforms = animatedModelData.combinedTransforms;
		for (int i = 0; i < animatedModelData.skeleton->bones.size(); ++i)
		{
			// Skip if bone is not in view frustum
			DirectX::XMFLOAT3 bonePos = {
				transforms[i].r[3].m128_f32[0],
				transforms[i].r[3].m128_f32[1],
				transforms[i].r[3].m128_f32[2]
			};

			// Bone pos in world space
			bonePos = DirectX::XMFLOAT3(
				GetPosition().x + bonePos.x * GetScale(),
				GetPosition().y + bonePos.y * GetScale(),
				GetPosition().z + bonePos.z * GetScale()
			);

			if (!aGfx.IsBoundingBoxInFrustum(bonePos, bonePos))
			{
				continue;
			}

			const int parentIndex = animatedModelData.skeleton->bones[i].parentIndex;

			if (parentIndex > 0)
			{
				DirectX::XMFLOAT3 parentPos = {
					transforms[parentIndex].r[3].m128_f32[0],
					transforms[parentIndex].r[3].m128_f32[1],
					transforms[parentIndex].r[3].m128_f32[2]
				};

				DirectX::XMFLOAT3 childPos = {
					transforms[i].r[3].m128_f32[0],
					transforms[i].r[3].m128_f32[1],
					transforms[i].r[3].m128_f32[2]
				};

				const DirectX::XMMATRIX projectionMatrix = transformConstantBuffer.GetTransforms(aGfx).objectToClip;

				// Transform bone positions to screen space
				DirectX::XMFLOAT2 screenParentPos;
				DirectX::XMFLOAT2 screenChildPos;

				DirectX::XMStoreFloat2(
					&screenParentPos,
					DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&parentPos), projectionMatrix)
				);

				DirectX::XMStoreFloat2(
					&screenChildPos,
					DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&childPos), projectionMatrix)
				);

				screenParentPos.x = (screenParentPos.x + 1.0f) * 0.5f * ImGui::GetIO().DisplaySize.x;
				screenParentPos.y = (1.0f - screenParentPos.y) * 0.5f * ImGui::GetIO().DisplaySize.y;
				screenChildPos.x = (screenChildPos.x + 1.0f) * 0.5f * ImGui::GetIO().DisplaySize.x;
				screenChildPos.y = (1.0f - screenChildPos.y) * 0.5f * ImGui::GetIO().DisplaySize.y;

				ImGui::GetForegroundDrawList()->AddCircle(
					ImVec2(screenChildPos.x, screenChildPos.y),
					4.0f,
					IM_COL32(0, 255, 0, 255)
				);

				ImGui::GetForegroundDrawList()->AddLine(
					ImVec2(screenParentPos.x, screenParentPos.y),
					ImVec2(screenChildPos.x, screenChildPos.y),
					IM_COL32(0, 255, 255, 255)
				);

				if (drawBoneNames)
				{
					// Draw bone name text imgui
					ImGui::GetForegroundDrawList()->AddText(
						ImVec2(screenChildPos.x, screenChildPos.y),
						IM_COL32(255, 255, 255, 255),
						animatedModelData.skeleton->bones[i].name.c_str()
					);
				}
			}
			else
			{
				DirectX::XMFLOAT3 rootPosition = {
					transforms[i].r[3].m128_f32[0],
					transforms[i].r[3].m128_f32[1],
					transforms[i].r[3].m128_f32[2]
				};

				const DirectX::XMMATRIX projectionMatrix = transformConstantBuffer.GetTransforms(aGfx).objectToClip;

				// Transform bone positions to screen space
				DirectX::XMFLOAT2 screenRootPos;

				DirectX::XMStoreFloat2(
					&screenRootPos,
					DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&rootPosition), projectionMatrix)
				);

				screenRootPos.x = (screenRootPos.x + 1.0f) * 0.5f * ImGui::GetIO().DisplaySize.x;
				screenRootPos.y = (1.0f - screenRootPos.y) * 0.5f * ImGui::GetIO().DisplaySize.y;

				ImGui::GetForegroundDrawList()->AddCircle(
					ImVec2(screenRootPos.x, screenRootPos.y),
					4.0f,
					IM_COL32(255, 255, 0, 255)
				);

				if (drawBoneNames)
				{
					// Draw bone name text imgui
					ImGui::GetForegroundDrawList()->AddText(
						ImVec2(screenRootPos.x, screenRootPos.y),
						IM_COL32(255, 255, 255, 255),
						animatedModelData.skeleton->bones[i].name.c_str()
					);
				}
			}
		}
	}

	void Model::UpdatePtr(float aDeltaTime)
	{
		if (!isLoaded)
		{
			return;
		}
		animationPlayer.Animate(aDeltaTime);
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
		pixelShader->Bind(aGfx);
	}

	DirectX::XMMATRIX& Model::GetBoneTransform(const int aBoneIndex)
	{
		return animatedModelData.finalTransforms[aBoneIndex];
	}

	DirectX::XMMATRIX& Model::GetBoneTransform(const std::string& aBoneName)
	{
		for (int i = 0; i < animatedModelData.skeleton->bones.size(); ++i)
		{
			if (animatedModelData.skeleton->bones[i].name == aBoneName)
			{
				return animatedModelData.finalTransforms[i];
			}
		}

		return animatedModelData.finalTransforms[0];
	}

	DirectX::XMMATRIX Model::GetBoneWorldTransform(const int aBoneIndex) const
	{
		return animatedModelData.combinedTransforms[aBoneIndex] * GetTransform();
	}

	void Model::SetPixelShader(const Graphics& aGfx, const std::wstring& aFilePath)
	{
		pixelShader = ShaderFactory::GetPixelShader(aGfx, aFilePath);
	}

	void Model::SetTexture(Texture* aTexture)
	{
		if (modelType == eModelType::Static)
		{
			modelData.texture = aTexture;
		}
		else if (modelType == eModelType::Skeletal)
		{
			animatedModelData.texture = aTexture;
		}
	}

	void Model::SetTextureAtIndex(Texture* aTexture, const size_t aIndex) const
	{
		if (modelType == eModelType::Static)
		{
			modelData.texture->SetTextureAtIndex(aTexture->GetTextureAtIndex(aIndex), aIndex);
		}
		else if (modelType == eModelType::Skeletal)
		{
			animatedModelData.texture->SetTextureAtIndex(aTexture->GetTextureAtIndex(aIndex), aIndex);
		}
	}

	void Model::ShowControlWindow(const char* aWindowName)
	{
		// Window name defaults to "Model"
		aWindowName = aWindowName ? aWindowName : "Model";

		if (ImGui::Begin(aWindowName))
		{
			ImGui::Columns(2, nullptr, true);

			//if (ImGui::Checkbox("1D Blend", &animationPlayer.useBlendTree)) {}

			if (!animationPlayer.useBlendTree)
			{
				ImGui::Text("Animations");
				for (int i = 0; i < animatedModelData.animationNames.size(); ++i)
				{
					if (ImGui::Button(animatedModelData.animationNames[i].c_str()))
					{
						animationPlayer.PlayAnimation(animatedModelData.animationNames[i]);
					}
				}

				if (animationPlayer.currentAnimation.isPlaying)
				{
					if (ImGui::Button("Pause"))
					{
						//animationPlayer.isAnimationPlaying = false;
						animationPlayer.PauseAnimation();
					}
				}
				else
				{
					if (ImGui::Button("Resume"))
					{
						//animationPlayer.isAnimationPlaying = true;
						animationPlayer.ResumeAnimation();
					}
				}

				if (ImGui::Button("Stop"))
				{
					//animationPlayer.isAnimationPlaying = false;
					animationPlayer.StopAnimation();
				}

				if (ImGui::Checkbox("Loop", &animationPlayer.currentAnimation.isLooping)) {}

				// Animation speed slider
				ImGui::Text("Speed");
				ImGui::SliderFloat("##Speed", &animationPlayer.currentAnimation.speed, 0.0f, 2.0f, "%.2f");
				ImGui::Text("Blend Time");
				ImGui::SliderFloat("##BlendTime", &animationPlayer.blendTime, 0.0f, 1.0f, "%.2f");
			}
			else
			{
				ImGui::Text("Blend Factor");
				ImGui::SliderFloat("##BlendFactor", &animationPlayer.blendFactor, 0.0f, 1.0f, "%.2f");
			}

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
			ImGui::Checkbox("Draw Skeleton", &drawSkeleton);
			ImGui::Checkbox("Draw Bone Names", &drawBoneNames);
		}
		ImGui::End();
	}
}
