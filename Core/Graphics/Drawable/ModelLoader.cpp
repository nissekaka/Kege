#include "ModelLoader.h"
//#include "Core/Model/Mesh.h"
#include "Core/Windows/Window.h"
#include "Core/Graphics/Drawable/Vertex.h"
#include <External/include/assimp/Importer.hpp>
#include <External/include/assimp/scene.h>
#include <External/include/assimp/postprocess.h>
#include <DirectXMath.h>
#include <cassert>
#include <vector>

namespace Kaka
{
	bool ModelLoader::LoadModel(const std::string& aFilePath, ModelData& aOutModelData)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(aFilePath,
		                                         aiProcess_Triangulate |
		                                         aiProcess_JoinIdenticalVertices |
		                                         aiProcess_FlipUVs |
		                                         aiProcess_ConvertToLeftHanded |
		                                         aiProcess_LimitBoneWeights |
		                                         aiProcess_GenSmoothNormals |
		                                         aiProcess_FindInvalidData
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Failed to load file: " << importer.GetErrorString() << std::endl;
			return false;
		}

		const aiMesh* mesh = scene->mMeshes[0];

		// Check if scene contains animations
		if (scene->mNumAnimations > 0)
		{
			// Load the skeleton and animations
			aOutModelData.skeleton = LoadSkeleton(scene);
			aOutModelData.animations = LoadAnimations(scene);
			// Store the default pose
			//for (const Bone& bone : aOutModelData.skeleton.bones)
			//{
			//	// Extract the bone's transform matrix from the aiMatrix4x4
			//	aOutModelData.defaultPose.push_back(bone.offsetMatrix);
			//}
			for (const Bone& bone : aOutModelData.skeleton.bones)
			{
				// Compute the default pose transformation matrix as the inverse of the bone's offset matrix
				DirectX::XMFLOAT4X4 defaultPoseTransform;
				DirectX::XMStoreFloat4x4(&defaultPoseTransform, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&bone.offsetMatrix)));

				// Store the default pose transformation matrix
				aOutModelData.defaultPose.push_back(defaultPoseTransform);
			}
			aOutModelData.modelType = eModelType::Skeletal;
			const std::string text = "\nSuccessfully loaded skeleton!"
				"\nNumber of bones: " + std::to_string(aOutModelData.skeleton.bones.size()) +
				"\nNumber of animations: " + std::to_string(aOutModelData.animations.size());
			OutputDebugStringA(text.c_str());
			std::string names;

			for (auto& anim : aOutModelData.animations)
			{
				names += "\n" + anim.name;
			}

			OutputDebugStringA(names.c_str());

			aOutModelData.animMesh.vertices.reserve(mesh->mNumVertices);

			for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				const DirectX::XMFLOAT3 position{mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z};
				const DirectX::XMFLOAT3 normal = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]);
				DirectX::XMFLOAT2 texCoord{0.0f,0.0f};
				DirectX::XMFLOAT3 tangent{0.0f,0.0f,0.0f};
				DirectX::XMFLOAT3 bitangent{0.0f,0.0f,0.0f};
				// Initialize bone indices and bone weights for the vertex
				std::array<unsigned int, 4> boneIndices{};
				std::array<float, 4> boneWeights{};

				// Check if the mesh has texture coordinates
				if (mesh->HasTextureCoords(0))
				{
					// Retrieve the first set of texture coordinates
					const aiVector3D& aiTexCoord = mesh->mTextureCoords[0][i];
					texCoord.x = aiTexCoord.x;
					texCoord.y = aiTexCoord.y;
				}

				if (mesh->HasTangentsAndBitangents())
				{
					tangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mTangents[i]);
					bitangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mBitangents[i]);
				}

				if (mesh->HasBones())
				{
					// Iterate over each bone influencing the current vertex
					for (unsigned int j = 0; j < mesh->mNumBones; ++j)
					{
						aiBone* bone = mesh->mBones[j];

						// Iterate over each vertex weight in the bone
						for (unsigned int k = 0; k < bone->mNumWeights; ++k)
						{
							aiVertexWeight vertexWeight = bone->mWeights[k];

							// Check if the vertex index matches the current vertex
							if (vertexWeight.mVertexId == i)
							{
								// Find an empty slot in boneIndices and boneWeights arrays
								for (unsigned int l = 0; l < boneIndices.size(); ++l)
								{
									if (boneWeights[l] == 0.0f)
									{
										// Store the bone index and weight for the vertex
										boneIndices[l] = j;
										boneWeights[l] = vertexWeight.mWeight;
										std::string boneText = "\n\nVertex: " + std::to_string(i) + "\nIndex: " + std::to_string(j) + "\nWeight: " + std::to_string(vertexWeight.mWeight);
										OutputDebugStringA(boneText.c_str());
										break;
									}
								}
							}
						}
					}
				}

				aOutModelData.animMesh.vertices.push_back({
					position,normal,texCoord,tangent,bitangent,boneIndices,boneWeights
				});
			}

			aOutModelData.animMesh.indices.reserve(
				static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(mesh->mNumFaces) *
				3);
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const auto& face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);
				aOutModelData.animMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[0]));
				aOutModelData.animMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[1]));
				aOutModelData.animMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[2]));
			}
		}
		else
		{
			aOutModelData.modelType = eModelType::Static;
			const std::string text = "\nNo skeleton found!";
			OutputDebugStringA(text.c_str());

			aOutModelData.mesh.vertices.reserve(mesh->mNumVertices);

			for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				const DirectX::XMFLOAT3 position{mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z};
				const DirectX::XMFLOAT3 normal = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]);
				DirectX::XMFLOAT2 texCoord{0.0f,0.0f};
				DirectX::XMFLOAT3 tangent{0.0f,0.0f,0.0f};
				DirectX::XMFLOAT3 bitangent{0.0f,0.0f,0.0f};

				// Check if the mesh has texture coordinates
				if (mesh->HasTextureCoords(0))
				{
					// Retrieve the first set of texture coordinates
					const aiVector3D& aiTexCoord = mesh->mTextureCoords[0][i];
					texCoord.x = aiTexCoord.x;
					texCoord.y = aiTexCoord.y;
				}

				if (mesh->HasTangentsAndBitangents())
				{
					tangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mTangents[i]);
					bitangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mBitangents[i]);
				}

				aOutModelData.mesh.vertices.push_back({position,normal,texCoord,tangent,bitangent});
			}

			aOutModelData.mesh.indices.reserve(
				static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(mesh->mNumFaces) *
				3);
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const auto& face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);
				aOutModelData.mesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[0]));
				aOutModelData.mesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[1]));
				aOutModelData.mesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[2]));
			}
		}

		return true;
	}

	Skeleton ModelLoader::LoadSkeleton(const aiScene* aScene)
	{
		Skeleton skeleton;

		// Iterate through each mesh
		for (unsigned int i = 0; i < aScene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = aScene->mMeshes[i];

			// Iterate through each bone in the mesh
			for (unsigned int j = 0; j < mesh->mNumBones; ++j)
			{
				const aiBone* bone = mesh->mBones[j];

				// Create a Bone struct and populate its properties
				Bone skeletonBone;
				skeletonBone.name = bone->mName.C_Str();
				skeletonBone.offsetMatrix = ConvertAssimpMatrixToDirectX(bone->mOffsetMatrix);

				// Add the bone to the skeleton
				skeleton.bones.push_back(skeletonBone);
			}
		}

		return skeleton;
	}

	// Helper function to find the index of the keyframe that corresponds to the given time
	unsigned int FindKeyframeIndex(const aiNodeAnim* aChannel, const float aTime)
	{
		// Iterate through the position keys to find the index of the keyframe that matches or is closest to the given time
		for (unsigned int i = 0; i < aChannel->mNumPositionKeys - 1; ++i)
		{
			if (aTime < aChannel->mPositionKeys[i + 1].mTime)
			{
				return i; // Return the index of the keyframe
			}
		}

		// If no keyframe was found, return the last keyframe index
		return aChannel->mNumPositionKeys - 1;
	}

	std::vector<AnimationClip> ModelLoader::LoadAnimations(const aiScene* aScene)
	{
		std::vector<AnimationClip> animations;

		// Iterate through each animation
		for (unsigned int i = 0; i < aScene->mNumAnimations; ++i)
		{
			const aiAnimation* animation = aScene->mAnimations[i];

			AnimationClip animationClip;
			animationClip.name = animation->mName.C_Str();

			unsigned int numFrames = 0; // Initialize the number of frames to zero

			// Calculate the duration of the animation in ticks
			float animationDuration = static_cast<float>(animation->mDuration);

			// Determine the maximum number of frames among all the channels
			for (unsigned int j = 0; j < animation->mNumChannels; ++j)
			{
				const aiNodeAnim* channel = animation->mChannels[j];
				numFrames = std::max(numFrames, channel->mNumPositionKeys);
			}

			// Calculate the duration of a single frame in ticks
			float frameDuration = animationDuration / (float)numFrames;

			// Iterate through each frame
			for (unsigned int frameIndex = 0; frameIndex < numFrames; ++frameIndex)
			{
				// Create a keyframe for the current frame
				Keyframe keyframe;
				keyframe.time = frameIndex * frameDuration;

				// Iterate through each channel (bone)
				for (unsigned int j = 0; j < animation->mNumChannels; ++j)
				{
					const aiNodeAnim* channel = animation->mChannels[j];

					// Find the position keyframe that corresponds to the current frame
					unsigned int positionIndex = FindKeyframeIndex(channel, keyframe.time);
					const aiVectorKey& positionKey = channel->mPositionKeys[positionIndex];

					// Find the rotation keyframe that corresponds to the current frame
					unsigned int rotationIndex = FindKeyframeIndex(channel, keyframe.time);
					const aiQuatKey& rotationKey = channel->mRotationKeys[rotationIndex];

					// Find the scaling keyframe that corresponds to the current frame
					unsigned int scalingIndex = FindKeyframeIndex(channel, keyframe.time);
					const aiVectorKey& scalingKey = channel->mScalingKeys[scalingIndex];

					// Create the bone transform matrix from the keyframe data
					DirectX::XMFLOAT4X4 boneTransform{};
					DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMMatrixIdentity());

					DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&boneTransform._41),
					                       DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&positionKey.mValue)));
					DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&boneTransform._31),
					                       DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&rotationKey.mValue)));
					DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&boneTransform._11),
					                       DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&scalingKey.mValue)));

					// Add the bone transform to the keyframe
					keyframe.boneTransforms.push_back(boneTransform);
				}

				// Add the keyframe to the animation clip
				animationClip.keyframes.push_back(keyframe);
			}

			// Add the animation clip to the animations vector
			animations.push_back(animationClip);
		}

		return animations;
	}

	DirectX::XMFLOAT4X4 ModelLoader::ConvertAssimpMatrixToDirectX(const aiMatrix4x4& aAssimpMatrix)
	{
		DirectX::XMFLOAT4X4 directxMatrix{};

		// Transpose the matrix since Assimp uses a row-major matrix representation
		directxMatrix._11 = aAssimpMatrix.a1;
		directxMatrix._12 = aAssimpMatrix.a2;
		directxMatrix._13 = aAssimpMatrix.a3;
		directxMatrix._14 = aAssimpMatrix.a4;

		directxMatrix._21 = aAssimpMatrix.b1;
		directxMatrix._22 = aAssimpMatrix.b2;
		directxMatrix._23 = aAssimpMatrix.b3;
		directxMatrix._24 = aAssimpMatrix.b4;

		directxMatrix._31 = aAssimpMatrix.c1;
		directxMatrix._32 = aAssimpMatrix.c2;
		directxMatrix._33 = aAssimpMatrix.c3;
		directxMatrix._34 = aAssimpMatrix.c4;

		directxMatrix._41 = aAssimpMatrix.d1;
		directxMatrix._42 = aAssimpMatrix.d2;
		directxMatrix._43 = aAssimpMatrix.d3;
		directxMatrix._44 = aAssimpMatrix.d4;

		return directxMatrix;
	}
}