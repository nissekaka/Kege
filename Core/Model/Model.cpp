#include "Model.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Model/ModelLoader.h"
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
	}

	void Model::Draw(const Graphics& aGfx)
	{
		if (!isLoaded)
		{
			return;
		}

		Sampler sampler(aGfx, 0u);
		sampler.Bind(aGfx);

		texture.Bind(aGfx);

		switch (modelData.modelType)
		{
		case eModelType::None:
		{
			assert("No model type!");
		}
		break;
		case eModelType::Static:
		{
			VertexBuffer vertexBuffer(aGfx, modelData.mesh.vertices);
			vertexBuffer.Bind(aGfx);

			IndexBuffer indexBuffer(aGfx, modelData.mesh.indices);
			indexBuffer.Bind(aGfx);
		}
		break;
		case eModelType::Skeletal:
		{
			VertexBuffer vertexBuffer(aGfx, modelData.animMesh.vertices);
			vertexBuffer.Bind(aGfx);

			IndexBuffer indexBuffer(aGfx, modelData.animMesh.indices);
			indexBuffer.Bind(aGfx);
		}
		break;
		default:
			assert("Error!");
		}

		TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		transformConstantBuffer.Bind(aGfx);

		switch (shaderType)
		{
		case eShaderType::Solid:
		{
			PixelShader pixelShader(aGfx, L"Shaders\\Solid_PS.cso");
			pixelShader.Bind(aGfx);

			struct PSMaterialConstant
			{
				DirectX::XMFLOAT4 colour;
			} pmc;
			pmc.colour = solidColour;

			PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
			psConstantBuffer.Bind(aGfx);

			VertexShader vertexShader(aGfx, L"Shaders\\Solid_VS.cso");
			vertexShader.Bind(aGfx);

			const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
			{
				{
					"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
					D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
				},
			};
			InputLayout inputLayout(aGfx, ied, vertexShader.GetBytecode());
			inputLayout.Bind(aGfx);
		}
		break;
		case eShaderType::Light:
		{
			PixelShader pixelShader(aGfx, L"Shaders\\Light_PS.cso");
			pixelShader.Bind(aGfx);

			struct PSMaterialConstant
			{
				BOOL normalMapEnabled = FALSE;
				BOOL materialEnabled = FALSE;
				BOOL padding1;
				BOOL padding2;
			} pmc;
			pmc.normalMapEnabled = texture.HasNormalMap();
			pmc.materialEnabled = texture.HasMaterial();

			PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
			psConstantBuffer.Bind(aGfx);

			VertexShader vertexShader(aGfx, L"Shaders\\Light_VS.cso");
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
		}
		break;
		case eShaderType::Phong:
		{
			PixelShader pixelShader(aGfx, L"Shaders\\Phong_PS.cso");
			pixelShader.Bind(aGfx);

			struct PSMaterialConstant
			{
				BOOL normalMapEnabled = FALSE;
				BOOL materialEnabled = FALSE;
				float specularIntensity = 0.1f;
				float specularPower = 30.0f;
			} pmc;
			pmc.normalMapEnabled = texture.HasNormalMap();
			pmc.materialEnabled = texture.HasMaterial();

			PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
			psConstantBuffer.Bind(aGfx);

			VertexShader vertexShader(aGfx, L"Shaders\\Phong_VS.cso");
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
		}
		break;
		case eShaderType::AnimPhong:
		{
			PixelShader pixelShader(aGfx, L"Shaders\\Phong_PS.cso");
			pixelShader.Bind(aGfx);

			struct PSMaterialConstant
			{
				BOOL normalMapEnabled = FALSE;
				BOOL materialEnabled = FALSE;
				float specularIntensity = 0.1f;
				float specularPower = 30.0f;
			} pmc;
			pmc.normalMapEnabled = texture.HasNormalMap();
			pmc.materialEnabled = texture.HasMaterial();

			PixelConstantBuffer<PSMaterialConstant> psConstantBuffer(aGfx, pmc, 0u);
			psConstantBuffer.Bind(aGfx);

			struct VSBoneConstant
			{
				DirectX::XMFLOAT4X4 bones[64u];
			} vsb;

			for (int i = 0; i < modelData.skeleton.bones.size(); ++i)
			{
				vsb.bones[i] = modelData.skeleton.bones[i].offsetMatrix;
			}

			VertexConstantBuffer<VSBoneConstant> vsConstantBuffer(aGfx, vsb, 1u);
			vsConstantBuffer.Bind(aGfx);

			VertexShader vertexShader(aGfx, L"Shaders\\AnimPhong_VS.cso");
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
				{
					"BLENDINDICES",0,DXGI_FORMAT_R8G8B8A8_UINT,0,
					D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
				},
				{
					"BLENDWEIGHT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
					D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
				},
			};
			InputLayout inputLayout(aGfx, ied, vertexShader.GetBytecode());
			inputLayout.Bind(aGfx);
		}
		break;
		}

		Topology topology(aGfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		topology.Bind(aGfx);

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
			float animationDuration = animation.keyframes.back().time;
			animationTime = fmodf(animationTime, animationDuration);
		}
	}

	DirectX::XMFLOAT3 TransformPosition(const DirectX::XMFLOAT3& aPosition, const DirectX::XMFLOAT4X4& aTransform)
	{
		DirectX::XMFLOAT4 transformedPosition;
		DirectX::XMStoreFloat4(&transformedPosition,
		                       DirectX::XMVector4Transform(DirectX::XMLoadFloat3(&aPosition),
		                                                   DirectX::XMLoadFloat4x4(&aTransform)));
		return DirectX::XMFLOAT3(transformedPosition.x, transformedPosition.y, transformedPosition.z);
	}

	DirectX::XMFLOAT3 TransformNormal(const DirectX::XMFLOAT3& aNormal, const DirectX::XMFLOAT4X4& aTransform)
	{
		DirectX::XMFLOAT3 transformedNormal;
		DirectX::XMStoreFloat3(&transformedNormal,
		                       DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&aNormal),
		                                                         DirectX::XMLoadFloat4x4(&aTransform)));
		return transformedNormal;
	}

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
				//size_t keyframeIndex2 = 0;
				for (size_t i = 0; i < animation.keyframes.size() - 1; ++i)
				{
					if (animation.keyframes[i + 1].time > animationTime)
					{
						keyframeIndex1 = i;
						//keyframeIndex2 = i + 1;
						break;
					}
				}

				// Interpolate between the two keyframes
				const Keyframe& keyframe1 = animation.keyframes[keyframeIndex1];
				//const Keyframe& keyframe2 = animation.keyframes[keyframeIndex2];
				//float t = (animationTime - keyframe1.time) / (keyframe2.time - keyframe1.time);

				// Interpolate bone transforms for each vertex
				for (auto& vertex : modelData.animMesh.vertices)
				{
					// Apply bone transforms based on bone indices and weights
					DirectX::XMFLOAT4X4 boneTransform{};
					DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMMatrixIdentity());

					DirectX::XMFLOAT4X4 accumulatedTransform{};
					DirectX::XMStoreFloat4x4(&accumulatedTransform, DirectX::XMLoadFloat4x4(&boneTransform));

					for (size_t i = 0; i < vertex.boneIndices.size(); ++i)
					{
						if (i > 0)
						{
							if (vertex.boneIndices[i] == vertex.boneIndices[i - 1])
							{
								continue;
							}
						}
						unsigned int boneIndex = vertex.boneIndices[i] + 1;
						float boneWeight = vertex.boneWeights[i];
						DirectX::XMFLOAT4X4 interpolatedTransform{};
						DirectX::XMStoreFloat4x4(&interpolatedTransform,
						                         //DirectX::XMMatrixMultiply(
						                         // DirectX::XMMatrixMultiply(
						                         //  DirectX::XMMatrixScaling(1.0f - t, 1.0f - t, 1.0f - t),
						                         DirectX::XMLoadFloat4x4(&keyframe1.boneTransforms[boneIndex])
						                         //),
						                         // DirectX::XMMatrixMultiply(
						                         //  DirectX::XMMatrixScaling(t, t, t),
						                         //  DirectX::XMLoadFloat4x4(&keyframe2.boneTransforms[boneIndex])
						                         // )
						                         //)
						);

						// Accumulate the interpolated bone transforms based on weights
						DirectX::XMFLOAT4X4 transformedBone{};
						DirectX::XMStoreFloat4x4(&transformedBone,
						                         DirectX::XMLoadFloat4x4(&interpolatedTransform) * boneWeight);

						DirectX::XMFLOAT4X4 newBoneTransform{};
						DirectX::XMStoreFloat4x4(&newBoneTransform,
						                         DirectX::XMMatrixMultiply(
							                         DirectX::XMLoadFloat4x4(&accumulatedTransform),
							                         DirectX::XMLoadFloat4x4(&transformedBone)
						                         ));

						accumulatedTransform = newBoneTransform;
					}

					// Apply the final bone transform to the vertex
					vertex.position = TransformPosition(vertex.position, accumulatedTransform);
					vertex.normal = TransformNormal(vertex.normal, accumulatedTransform);
					vertex.tangent = TransformNormal(vertex.tangent, accumulatedTransform);
					vertex.bitangent = TransformNormal(vertex.bitangent, accumulatedTransform);
				}
			}
			else
			{
				// Play paused frame
			}
		}
		else
		{
			// Apply the default pose or any other static transformation when the animation is not playing
			for (auto& vertex : modelData.animMesh.vertices)
			{
				// Initialize the bone transform as the identity matrix
				DirectX::XMFLOAT4X4 boneTransform{};
				DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMMatrixIdentity());

				// Apply bone transforms based on bone indices and weights
				for (size_t i = 0; i < vertex.boneIndices.size(); ++i)
				{
					unsigned int boneIndex = vertex.boneIndices[i];

					// Use the default pose transform for the corresponding bone index
					const DirectX::XMFLOAT4X4& defaultPoseTransform = modelData.skeleton.bones[boneIndex].offsetMatrix;

					// Accumulate the default pose bone transform to the overall bone transform
					DirectX::XMFLOAT4X4 weightedDefaultPoseTransform;
					DirectX::XMStoreFloat4x4(&weightedDefaultPoseTransform,
					                         DirectX::XMLoadFloat4x4(&defaultPoseTransform));

					// Accumulate the default pose bone transform to the overall bone transform
					DirectX::XMStoreFloat4x4(&boneTransform,
					                         DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&boneTransform),
					                                                   DirectX::XMLoadFloat4x4(
						                                                   &weightedDefaultPoseTransform)));
				}

				// Apply the final bone transform to the vertex
				vertex.position = TransformPosition(vertex.position, boneTransform);
				vertex.normal = TransformNormal(vertex.normal, boneTransform);
				vertex.tangent = TransformNormal(vertex.tangent, boneTransform);
				vertex.bitangent = TransformNormal(vertex.bitangent, boneTransform);
			}

			//// Apply the default pose or any other static transformation when the animation is not playing
			//for (auto& vertex : modelData.animMesh.vertices)
			//{
			//	// Initialize the bone transform as the identity matrix
			//	DirectX::XMFLOAT4X4 boneTransform{};
			//	DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMMatrixIdentity());

			//	// Apply bone transforms based on bone indices and weights
			//	for (size_t i = 0; i < 4; ++i)
			//	{
			//		unsigned int boneIndex = vertex.boneIndices[i];
			//		float boneWeight = vertex.boneWeights[i];

			//		// Use the default pose transform for the corresponding bone index
			//		const DirectX::XMFLOAT4X4& defaultPoseTransform = modelData.defaultPose[boneIndex];

			//		// Accumulate the default pose bone transforms based on weights
			//		DirectX::XMFLOAT4X4 weightedDefaultPoseTransform;
			//		DirectX::XMStoreFloat4x4(&weightedDefaultPoseTransform,
			//		                         DirectX::XMLoadFloat4x4(&defaultPoseTransform) * boneWeight);

			//		// Accumulate the default pose bone transform to the overall bone transform
			//		DirectX::XMStoreFloat4x4(&boneTransform,
			//		                         DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&boneTransform),
			//		                                                   DirectX::XMLoadFloat4x4(
			//			                                                   &weightedDefaultPoseTransform)));
			//	}

			//	// Apply the final bone transform to the vertex
			//	vertex.position = TransformPosition(vertex.position, boneTransform);
			//	vertex.normal = TransformNormal(vertex.normal, boneTransform);
			//	vertex.tangent = TransformNormal(vertex.tangent, boneTransform);
			//	vertex.bitangent = TransformNormal(vertex.bitangent, boneTransform);
			//}
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
			ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			ImGui::Text("Scale");
			ImGui::SliderFloat("XYZ", &transform.scale, 0.0f, 5.0f, nullptr, ImGuiSliderFlags_Logarithmic);
		}
		ImGui::End();
	}
}