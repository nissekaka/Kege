#include "Model.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Drawable/ModelLoader.h"
#include <External/include/imgui/imgui.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

namespace Kaka
{
	Model::Model(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType)
		:
		shaderType(aShaderType)
	{
		isLoaded = ModelLoader::LoadModel(aFilePath, modelData);
		texture.LoadTextureFromModel(aGfx, aFilePath);
		solidColour = {1.0f,1.0f,1.0f,1.0f};
	}

	void Model::LoadModel(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType)
	{
		shaderType = aShaderType;

		isLoaded = ModelLoader::LoadModel(aFilePath, modelData);
		texture.LoadTextureFromModel(aGfx, aFilePath);
		solidColour = {1.0f,1.0f,1.0f,1.0f};

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
						"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
						D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
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
			}
			break;
			case eShaderType::Phong:
			{
				pixelShader.Init(aGfx, L"Shaders\\Phong_PS.cso");
				vertexShader.Init(aGfx, L"Shaders\\Phong_VS.cso");

				ied =
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
					{
						"BLENDINDICES",0,DXGI_FORMAT_R8G8B8A8_UINT,0,
						D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
					},
					{
						"BLENDWEIGHT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
						D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
					},
				};
			}
			break;
			}

			inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
			topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			rasterizer.Init(aGfx, false);
			depthStencil.Init(aGfx, DepthStencil::Mode::Write);
		}
	}

	void Model::Draw(const Graphics& aGfx)
	{
		if (!isLoaded)
		{
			return;
		}

		sampler.Bind(aGfx);
		texture.Bind(aGfx);

		if (modelData.modelType == eModelType::Skeletal)
		{
			vertexBuffer.Init(aGfx, modelData.animMesh.vertices);
		}

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
				vsb.bones[i] = modelData.skeleton.bones[i].offsetMatrix;
			}

			VertexConstantBuffer<VSBoneConstant> vsConstantBuffer(aGfx, vsb, 1u);
			vsConstantBuffer.Bind(aGfx);
		}
		break;
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

		aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[1] = {nullptr};
		aGfx.pContext->PSSetShaderResources(0, 1, nullSRVs);
		aGfx.pContext->PSSetShaderResources(1, 1, nullSRVs);
		aGfx.pContext->PSSetShaderResources(2, 1, nullSRVs);
	}

	void Model::Update(const float aDeltaTime)
	{
		if (modelData.modelType != eModelType::Skeletal)
		{
			return;
		}
		if (selectedAnimationIndex >= 0 && selectedAnimationIndex < modelData.animations.size() && isAnimationPlaying)
		{
			AnimationClip& animation = modelData.animations[selectedAnimationIndex];

			// Update animation time based on the animation speed
			animationTime += aDeltaTime * animationSpeed;

			// Wrap the animation time within the animation duration
			float animationDuration = animation.keyframes.back().time - animation.keyframes.front().time;
			animationTime = fmodf(animationTime, animationDuration);
		}
	}

	//DirectX::XMFLOAT3 TransformPosition(const DirectX::XMFLOAT3& aPosition, const DirectX::XMFLOAT4X4& aTransform)
	//{
	//	DirectX::XMFLOAT4 transformedPosition;
	//	DirectX::XMStoreFloat4(&transformedPosition,
	//	                       DirectX::XMVector4Transform(DirectX::XMLoadFloat3(&aPosition),
	//	                                                   DirectX::XMLoadFloat4x4(&aTransform)));
	//	return DirectX::XMFLOAT3(transformedPosition.x, transformedPosition.y, transformedPosition.z);
	//}

	//DirectX::XMFLOAT3 TransformNormal(const DirectX::XMFLOAT3& aNormal, const DirectX::XMFLOAT4X4& aTransform)
	//{
	//	DirectX::XMFLOAT3 transformedNormal;
	//	DirectX::XMStoreFloat3(&transformedNormal,
	//	                       DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&aNormal),
	//	                                                         DirectX::XMLoadFloat4x4(&aTransform)));
	//	return transformedNormal;
	//}

	//DirectX::XMFLOAT3 TransformTangent(const DirectX::XMFLOAT3& aTangent, const DirectX::XMFLOAT4X4& aTransform)
	//{
	//	DirectX::XMFLOAT3 transformedTangent;
	//	DirectX::XMStoreFloat3(&transformedTangent,
	//	                       DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&aTangent),
	//	                                                         DirectX::XMLoadFloat4x4(&aTransform)));
	//	return transformedTangent;
	//}

	//DirectX::XMFLOAT3 TransformBitangent(const DirectX::XMFLOAT3& aBitangent, const DirectX::XMFLOAT4X4& aTransform)
	//{
	//	DirectX::XMFLOAT3 transformedBitangent;
	//	DirectX::XMStoreFloat3(&transformedBitangent,
	//	                       DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&aBitangent),
	//	                                                         DirectX::XMLoadFloat4x4(&aTransform)));
	//	return transformedBitangent;
	//}

	void Model::Animate()
	{
		if (modelData.modelType != eModelType::Skeletal)
		{
			return;
		}
		if (selectedAnimationIndex >= 0 && selectedAnimationIndex < modelData.animations.size())
		{
			const AnimationClip& animation = modelData.animations[selectedAnimationIndex];

			// Apply animation transformation based on the current time
			if (isAnimationPlaying)
			{
				// Find the two keyframes for interpolation
				size_t keyframeIndex1 = 0;
				size_t keyframeIndex2 = 0;
				for (size_t i = 0; i < animation.keyframes.size() - 1; ++i)
				{
					if (animation.keyframes[i + 1].time > animationTime)
					{
						keyframeIndex1 = i;
						keyframeIndex2 = i + 1;
						break;
					}
				}

				std::string time = "\nAnim time: " + std::to_string(animationTime);
				OutputDebugStringA(time.c_str());

				// Interpolate between the two keyframes
				const Keyframe& keyframe1 = animation.keyframes[keyframeIndex1];
				const Keyframe& keyframe2 = animation.keyframes[keyframeIndex2];
				float t = std::clamp((animationTime - keyframe1.time) / (keyframe2.time - keyframe1.time), 0.0f, 1.0f);
				// Calculate the inverse of the bind pose transformation matrices
				std::vector<DirectX::XMFLOAT4X4> invBindPoseTransforms(modelData.skeleton.bones.size());

				for (size_t i = 0; i < invBindPoseTransforms.size(); ++i)
				{
					const DirectX::XMFLOAT4X4& bindPoseTransform = modelData.bindPose[i];
					DirectX::XMStoreFloat4x4(&invBindPoseTransforms[i], DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&bindPoseTransform)));
				}

				// Calculate the bone transforms for each keyframe bone
				std::vector<DirectX::XMFLOAT4X4> interpolatedBoneTransforms(modelData.skeleton.bones.size());

				for (size_t i = 0; i < interpolatedBoneTransforms.size(); ++i)
				{
					const DirectX::XMFLOAT4X4& transform1 = keyframe1.boneTransforms[i];
					const DirectX::XMFLOAT4X4& transform2 = keyframe2.boneTransforms[i];

					DirectX::XMFLOAT4X4 interpolatedTransform{};
					for (int row = 0; row < 4; ++row)
					{
						for (int col = 0; col < 4; ++col)
						{
							interpolatedTransform.m[row][col] = transform1.m[row][col] + t * (transform2.m[row][col] - transform1.m[row][col]);
						}
					}

					DirectX::XMFLOAT4X4 finalBoneTransform;
					DirectX::XMStoreFloat4x4(&finalBoneTransform, DirectX::XMMatrixMultiply(
						                         DirectX::XMLoadFloat4x4(&interpolatedTransform),
						                         DirectX::XMLoadFloat4x4(&invBindPoseTransforms[i])));

					interpolatedBoneTransforms[i] = finalBoneTransform;
				}

				// Update the bone transformations in the skeleton
				for (size_t i = 0; i < modelData.skeleton.bones.size(); ++i)
				{
					DirectX::XMFLOAT4X4& boneTransform = modelData.skeleton.bones[i].offsetMatrix;
					DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMLoadFloat4x4(&interpolatedBoneTransforms[i]));
				}
			}
			else
			{
				// Play paused frame
			}
		}
		else
		{
			// Update the bone transformations in the skeleton
			for (size_t i = 0; i < modelData.skeleton.bones.size(); ++i)
			{
				DirectX::XMFLOAT4X4& bindPoseTransform = modelData.bindPose[i];
				DirectX::XMFLOAT4X4& boneTransform = modelData.skeleton.bones[i].offsetMatrix;
				DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMLoadFloat4x4(&bindPoseTransform));
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
		return {transform.x,transform.y,transform.z};
	}

	DirectX::XMFLOAT3 Model::GetRotation() const
	{
		return {transform.roll,transform.pitch,transform.yaw};
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