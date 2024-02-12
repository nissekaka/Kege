#include "stdafx.h"
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

#include "TGAFBXImporter/source/FBXImporter.h"

namespace Kaka
{
	//bool ModelLoader::LoadModel(const std::string& aFilePath, ModelData& aOutModelData)
	//{
	//	Assimp::Importer importer;
	//	const aiScene* scene = importer.ReadFile(aFilePath,
	//	                                         aiProcess_Triangulate |
	//	                                         aiProcess_JoinIdenticalVertices |
	//	                                         aiProcess_FlipUVs |
	//	                                         aiProcess_ConvertToLeftHanded |
	//	                                         aiProcess_LimitBoneWeights |
	//	                                         /*aiProcess_GenSmoothNormals |*/
	//	                                         aiProcess_FindInvalidData |
	//	                                         aiProcessPreset_TargetRealtime_Quality
	//	);

	//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	//	{
	//		std::cerr << "Failed to load file: " << importer.GetErrorString() << std::endl;
	//		return false;
	//	}

	//	const aiMesh* mesh = scene->mMeshes[0];

	//	//aOutModelData.globalInverseMatrix = AssimpToDirectXMatrix(scene->mRootNode->mTransformation);
	//	//DirectX::XMStoreFloat4x4(&aOutModelData.globalInverseMatrix,
	//	//                         DirectX::XMMatrixInverse(
	//	//	                         nullptr, DirectX::XMLoadFloat4x4(&aOutModelData.globalInverseMatrix)));

	//	// Check if scene contains animations
	//	if (scene->mNumAnimations > 0)
	//	{
	//		// Load the skeleton and animations
	//		aOutModelData.skeleton = LoadSkeleton(scene);
	//		aOutModelData.animations = LoadAnimations(scene);

	//		for (const Bone& bone : aOutModelData.skeleton.bones)
	//		{
	//			// Compute the bind pose transformation matrix as the inverse of the bone's offset matrix
	//			DirectX::XMMATRIX bindPose = DirectX::XMMatrixInverse(nullptr, bone.bindPose);


	//			// Store the bind pose transformation matrix
	//			//aOutModelData.bindPose.push_back(bindPoseBoneTransform);
	//		}

	//		aOutModelData.modelType = eModelType::Skeletal;

	//		const std::string text = "\nSuccessfully loaded skeleton!"
	//			"\nNumber of bones: " + std::to_string(aOutModelData.skeleton.bones.size()) +
	//			"\nNumber of animations: " + std::to_string(aOutModelData.animations.size());
	//		OutputDebugStringA(text.c_str());
	//		std::string names;

	//		for (auto& anim : aOutModelData.animations)
	//		{
	//			names += "\n" + anim.name;
	//		}

	//		OutputDebugStringA(names.c_str());

	//		aOutModelData.animMesh.vertices.reserve(mesh->mNumVertices);

	//		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	//		{
	//			const DirectX::XMFLOAT3 position{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
	//			const DirectX::XMFLOAT3 normal = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]);
	//			DirectX::XMFLOAT2 texCoord{0.0f, 0.0f};
	//			DirectX::XMFLOAT3 tangent{0.0f, 0.0f, 0.0f};
	//			DirectX::XMFLOAT3 bitangent{0.0f, 0.0f, 0.0f};
	//			// Initialize bone indices and bone weights for the vertex
	//			std::array<unsigned int, 4> boneIndices = {};
	//			std::array<float, 4> boneWeights = {};

	//			// Check if the mesh has texture coordinates
	//			if (mesh->HasTextureCoords(0))
	//			{
	//				// Retrieve the first set of texture coordinates
	//				const aiVector3D& aiTexCoord = mesh->mTextureCoords[0][i];
	//				texCoord.x = aiTexCoord.x;
	//				texCoord.y = aiTexCoord.y;
	//			}

	//			if (mesh->HasTangentsAndBitangents())
	//			{
	//				tangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mTangents[i]);
	//				bitangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mBitangents[i]);
	//			}

	//			if (mesh->HasBones())
	//			{
	//				// Keep track of the number of bone influences encountered
	//				unsigned int numInfluences = 0;

	//				// Iterate over each bone influencing the current vertex
	//				for (unsigned int j = 0; j < mesh->mNumBones; ++j)
	//				{
	//					aiBone* bone = mesh->mBones[j];

	//					// Iterate over each vertex weight in the bone
	//					for (unsigned int k = 0; k < bone->mNumWeights; ++k)
	//					{
	//						aiVertexWeight vertexWeight = bone->mWeights[k];

	//						// Check if the vertex index matches the current vertex
	//						if (vertexWeight.mVertexId == i)
	//						{
	//							// Find the index for the bone in the boneIndices vector
	//							auto it = std::find(boneIndices.begin(), boneIndices.end(), j);
	//							if (it == boneIndices.end())
	//							{
	//								// The bone index is not already in the boneIndices vector, add it
	//								if (numInfluences < boneIndices.size())
	//								{
	//									boneIndices[numInfluences] = j;
	//									boneWeights[numInfluences] = vertexWeight.mWeight;
	//									numInfluences++;
	//								}
	//							}
	//							else
	//							{
	//								// The bone index already exists in the boneIndices vector, update its weight
	//								size_t index = std::distance(boneIndices.begin(), it);
	//								boneWeights[index] = vertexWeight.mWeight;
	//							}
	//						}
	//					}
	//				}

	//				// Normalize bone weights if there are multiple influences
	//				if (numInfluences > 1)
	//				{
	//					// Calculate the sum of bone weights
	//					float weightSum = 0.0f;
	//					for (unsigned int l = 0; l < numInfluences; ++l)
	//					{
	//						weightSum += boneWeights[l];
	//					}

	//					// Normalize the bone weights
	//					for (unsigned int l = 0; l < numInfluences; ++l)
	//					{
	//						boneWeights[l] /= weightSum;
	//					}
	//				}
	//			}

	//			aOutModelData.animMesh.vertices.push_back({
	//				position, texCoord, normal, tangent, bitangent,
	//				{boneIndices[0], boneIndices[1], boneIndices[2], boneIndices[3]},
	//				{boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]}
	//			});

	//			//for (int k = 0; k < 4; ++k)
	//			//{
	//			//	std::string boneInfo = "\n Vertex: " + std::to_string(i) + " [" + std::to_string(k) + "] Bone: " + std::to_string(boneIndices[k]) + " Weight: " + std::to_string(boneWeights[k]);
	//			//	OutputDebugStringA(boneInfo.c_str());
	//			//}
	//			//OutputDebugStringA("\n---");
	//		}

	//		aOutModelData.animMesh.indices.reserve(
	//			static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(mesh->mNumFaces) *
	//			3);
	//		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	//		{
	//			const auto& face = mesh->mFaces[i];
	//			assert(face.mNumIndices == 3);
	//			aOutModelData.animMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[0]));
	//			aOutModelData.animMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[1]));
	//			aOutModelData.animMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[2]));
	//		}
	//	}
	//	else
	//	{
	//		aOutModelData.modelType = eModelType::Static;
	//		const std::string text = "\nNo skeleton found!";
	//		OutputDebugStringA(text.c_str());

	//		aOutModelData.mesh.vertices.reserve(mesh->mNumVertices);

	//		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	//		{
	//			const DirectX::XMFLOAT3 position{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
	//			const DirectX::XMFLOAT3 normal = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]);
	//			DirectX::XMFLOAT2 texCoord{0.0f, 0.0f};
	//			DirectX::XMFLOAT3 tangent{0.0f, 0.0f, 0.0f};
	//			DirectX::XMFLOAT3 bitangent{0.0f, 0.0f, 0.0f};

	//			// Check if the mesh has texture coordinates
	//			if (mesh->HasTextureCoords(0))
	//			{
	//				// Retrieve the first set of texture coordinates
	//				const aiVector3D& aiTexCoord = mesh->mTextureCoords[0][i];
	//				texCoord.x = aiTexCoord.x;
	//				texCoord.y = aiTexCoord.y;
	//			}

	//			if (mesh->HasTangentsAndBitangents())
	//			{
	//				tangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mTangents[i]);
	//				bitangent = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mBitangents[i]);
	//			}

	//			aOutModelData.mesh.vertices.push_back({position, texCoord, normal, tangent, bitangent});
	//		}

	//		aOutModelData.mesh.indices.reserve(
	//			static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(mesh->mNumFaces) *
	//			3);
	//		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	//		{
	//			const auto& face = mesh->mFaces[i];
	//			assert(face.mNumIndices == 3);
	//			aOutModelData.mesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[0]));
	//			aOutModelData.mesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[1]));
	//			aOutModelData.mesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[2]));
	//		}
	//	}

	//	return true;
	//}

	//Skeleton ModelLoader::LoadSkeleton(const aiScene* aScene)
	//{
	//	Skeleton skeleton;

	//	for (unsigned int i = 0; i < aScene->mNumMeshes; ++i)
	//	{
	//		const aiMesh* mesh = aScene->mMeshes[i];

	//		for (unsigned int j = 0; j < mesh->mNumBones; ++j)
	//		{
	//			const aiBone* bone = mesh->mBones[j];

	//			Bone skeletonBone;
	//			skeletonBone.name = bone->mName.C_Str();
	//			//skeletonBone.bindPose = AssimpToDirectXMatrix(bone->mOffsetMatrix);

	//			skeleton.bones.push_back(skeletonBone);
	//		}
	//	}

	//	for (unsigned int i = 0; i < skeleton.bones.size(); ++i)
	//	{
	//		const aiNode* boneNode = aScene->mRootNode->FindNode(skeleton.bones[i].name.c_str());
	//		if (boneNode)
	//		{
	//			const aiNode* parentNode = boneNode->mParent;
	//			if (parentNode)
	//			{
	//				std::string parentName = parentNode->mName.C_Str();
	//				for (unsigned int j = 0; j < skeleton.bones.size(); ++j)
	//				{
	//					if (skeleton.bones[j].name == parentName)
	//					{
	//						skeleton.bones[i].parentIndex = j;
	//						//skeleton.bones[j].childIndices.push_back(i);
	//						break;
	//					}
	//				}
	//			}
	//		}
	//	}

	//	// Find and set the rootBoneIndex
	//	for (int i = 0; i < skeleton.bones.size(); ++i)
	//	{
	//		if (skeleton.bones[i].parentIndex == -1)
	//		{
	//			skeleton.rootBoneIndex = i;
	//			break;
	//		}
	//	}

	//	return skeleton;
	//}

	//// Helper function to find the index of the keyframe that corresponds to the given time
	//unsigned int FindKeyframeIndex(const aiNodeAnim* aChannel, const float aTime)
	//{
	//	const unsigned int numKeys = aChannel->mNumPositionKeys;

	//	// Handle cases where the given time is greater than the last keyframe time
	//	if (aTime >= aChannel->mPositionKeys[numKeys - 1].mTime)
	//	{
	//		return numKeys - 1; // Return the index of the last keyframe
	//	}

	//	// Iterate through the position keys to find the index of the keyframe that matches or is closest to the given time
	//	for (unsigned int i = 0; i < numKeys - 1; ++i)
	//	{
	//		if (aTime < aChannel->mPositionKeys[i + 1].mTime)
	//		{
	//			return i; // Return the index of the keyframe
	//		}
	//	}

	//	// If no keyframe was found, return the last keyframe index
	//	return numKeys - 1;
	//}

	//std::vector<AnimationClip> ModelLoader::LoadAnimations(const aiScene* aScene)
	//{
	//	std::vector<AnimationClip> animations;

	//	// Iterate through each animation
	//	for (unsigned int i = 0; i < aScene->mNumAnimations; ++i)
	//	{
	//		const aiAnimation* animation = aScene->mAnimations[i];

	//		AnimationClip animationClip;
	//		animationClip.name = animation->mName.C_Str();

	//		const float ticksPerSecond = animation->mTicksPerSecond != 0
	//			                             ? static_cast<float>(animation->mTicksPerSecond)
	//			                             : 25.0f;
	//		const float timeDuration = static_cast<float>(animation->mDuration) / ticksPerSecond;

	//		// Determine the maximum number of frames among all the channels
	//		unsigned int numFrames = 0;
	//		for (unsigned int j = 1; j < animation->mNumChannels; ++j)
	//		{
	//			const aiNodeAnim* channel = animation->mChannels[j];
	//			numFrames = (std::max)(numFrames, channel->mNumPositionKeys);
	//		}

	//		// Calculate the duration of a single frame in seconds
	//		float frameDuration = timeDuration / static_cast<float>(numFrames);

	//		// Iterate through each frame
	//		for (unsigned int frameIndex = 0; frameIndex < numFrames; ++frameIndex)
	//		{
	//			// Create a keyframe for the current frame
	//			Keyframe keyframe;
	//			keyframe.time = (float)frameIndex * frameDuration;

	//			// Iterate through each channel (bone)
	//			for (unsigned int j = 1; j < animation->mNumChannels; ++j)
	//			{
	//				const aiNodeAnim* channel = animation->mChannels[j];

	//				//const unsigned int keyframeIndex = FindKeyframeIndex(channel, keyframe.time);
	//				const aiVectorKey& positionKey = channel->mPositionKeys[frameIndex];
	//				//const aiQuatKey& rotationKey = channel->mRotationKeys[frameIndex];
	//				//const aiVectorKey& scalingKey = channel->mScalingKeys[frameIndex];


	//				DirectX::XMFLOAT3 position(positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z);
	//				std::string posKey = "\nIndex: " + std::to_string(frameIndex) + "  Pos: " +
	//					std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(
	//						position.z);
	//				OutputDebugStringA(posKey.c_str());
	//				//DirectX::XMFLOAT4 rotation(rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z, rotationKey.mValue.w);
	//				//DirectX::XMFLOAT3 scaling(scalingKey.mValue.x, scalingKey.mValue.y, scalingKey.mValue.z);
	//				DirectX::XMFLOAT4 rotation(0.0f, 0.0f, 0.0f, 0.0f);
	//				DirectX::XMFLOAT3 scaling(1.0f, 1.0f, 1.0f);

	//				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(
	//					DirectX::XMLoadFloat3(&position));
	//				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(
	//					DirectX::XMLoadFloat4(&rotation));
	//				DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixScalingFromVector(
	//					DirectX::XMLoadFloat3(&scaling));

	//				DirectX::XMMATRIX boneTransformMatrix = translationMatrix * rotationMatrix * scalingMatrix;

	//				// Add the bone transform to the keyframe
	//				keyframe.boneTransforms.push_back(boneTransformMatrix);
	//			}

	//			// Add the keyframe to the animation clip
	//			animationClip.keyframes.push_back(keyframe);
	//		}

	//		// Add the animation clip to the animations vector
	//		animations.push_back(animationClip);
	//	}

	//	return animations;
	//}

	//DirectX::XMFLOAT4X4 ModelLoader::AssimpToDirectXMatrix(const aiMatrix4x4& aAssimpMatrix)
	//{
	//	return {
	//		aAssimpMatrix.a1, aAssimpMatrix.b1, aAssimpMatrix.c1, aAssimpMatrix.d1,
	//		aAssimpMatrix.a2, aAssimpMatrix.b2, aAssimpMatrix.c2, aAssimpMatrix.d2,
	//		aAssimpMatrix.a3, aAssimpMatrix.b3, aAssimpMatrix.c3, aAssimpMatrix.d3,
	//		aAssimpMatrix.a4, aAssimpMatrix.b4, aAssimpMatrix.c4, aAssimpMatrix.d4
	//	};
	//}

	bool ModelLoader::LoadModel(ModelDataPtr& aOutModelData, const std::string& aFilePath)
	{
		TGA::FBXModel fbxModel;

		if (TGA::FBXImporter::LoadModel(aFilePath, fbxModel))
		{
			meshLists[aFilePath] = MeshList();
			MeshList& meshList = meshLists[aFilePath];
			aOutModelData.meshList = &meshList;

			meshLists[aFilePath].meshes.resize(fbxModel.Meshes.size());

			// Copy model data from FBXImporter to our own model data
			aOutModelData.meshList = &meshLists[aFilePath];
			for (size_t i = 0; i < aOutModelData.meshList->meshes.size(); ++i)
			{
				// Imported data
				TGA::FBXModel::FBXMesh& fbxMesh = fbxModel.Meshes[i];

				// Our own data
				Mesh& mesh = aOutModelData.meshList->meshes[i];

				std::vector<Vertex> vertices;
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

					vertices[v].texCoord.x = fbxMesh.Vertices[v].UVs[0][0];
					vertices[v].texCoord.y = fbxMesh.Vertices[v].UVs[0][1];
				}

				mesh.vertices = vertices;

				for (const auto& index : fbxMesh.Indices)
				{
					mesh.indices.push_back(index);
				}

				// Assign material name
				aOutModelData.meshList->materialNames.push_back(fbxModel.Materials[fbxMesh.MaterialIndex].MaterialName);
			}
			return true;
		}
		return false;
	}

	bool ModelLoader::LoadAnimatedModel(AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath)
	{
		if (skeletons.contains(aFilePath))
		{
			aOutModelData.skeleton = &skeletons[aFilePath];

			if (animatedMeshLists.contains(aFilePath))
			{
				aOutModelData.meshList = &animatedMeshLists[aFilePath];
			}

			aOutModelData.combinedTransforms.resize(aOutModelData.skeleton->bones.size());
			aOutModelData.finalTransforms.resize(aOutModelData.skeleton->bones.size());

			for (unsigned int i = 0; i < aOutModelData.skeleton->bones.size(); i++)
			{
				aOutModelData.combinedTransforms[i] = aOutModelData.skeleton->bones[i].bindPose;
				aOutModelData.finalTransforms[i] = aOutModelData.skeleton->bones[i].bindPose;
			}

			return true;
		}

		TGA::FBXModel fbxModel;

		if (TGA::FBXImporter::LoadModel(aFilePath, fbxModel))
		{
			skeletons[aFilePath] = Skeleton();
			Skeleton& skeleton = skeletons[aFilePath];
			aOutModelData.skeleton = &skeleton;

			animatedMeshLists[aFilePath] = AnimatedMeshList();
			AnimatedMeshList& animatedMeshList = animatedMeshLists[aFilePath];
			aOutModelData.meshList = &animatedMeshList;

			// Copy bone data from FBXImporter to our own model data
			for (auto& bone : fbxModel.Skeleton.Bones)
			{
				aOutModelData.skeleton->bones.emplace_back();
				auto& newBone = aOutModelData.skeleton->bones.back();

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
				aOutModelData.skeleton->boneNames.push_back(bone.Name);

				// Add bone offset matrix to bind pose
				aOutModelData.combinedTransforms.push_back(newBone.bindPose);
				aOutModelData.finalTransforms.push_back(newBone.bindPose);
			}

			animatedMeshLists[aFilePath].meshes.resize(fbxModel.Meshes.size());

			// Copy model data from FBXImporter to our own model data
			aOutModelData.meshList = &animatedMeshLists[aFilePath];
			for (size_t i = 0; i < aOutModelData.meshList->meshes.size(); ++i)
			{
				// Imported data
				TGA::FBXModel::FBXMesh& fbxMesh = fbxModel.Meshes[i];

				// Our own data
				AnimatedMesh& mesh = aOutModelData.meshList->meshes[i];

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

				// Assign material name
				aOutModelData.meshList->materialNames.push_back(fbxModel.Materials[fbxMesh.MaterialIndex].MaterialName);
			}
			return true;
		}
		return false;
	}

	bool ModelLoader::LoadTexture(const Graphics& aGfx, AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath)
	{
		if (textures.contains(aFilePath))
		{
			aOutModelData.texture = &textures[aFilePath];
			return true;
		}

		textures[aFilePath] = Texture(2u);
		textures[aFilePath].LoadTextureFromModel(aGfx, aFilePath);
		aOutModelData.texture = &textures[aFilePath];

		return true;
	}

	bool ModelLoader::LoadTexture(const Graphics& aGfx, ModelDataPtr& aOutModelData, const std::string& aFilePath)
	{
		if (textures.contains(aFilePath))
		{
			aOutModelData.texture = &textures[aFilePath];
			return true;
		}

		textures[aFilePath] = Texture(2u);
		textures[aFilePath].LoadTextureFromModel(aGfx, aFilePath);
		aOutModelData.texture = &textures[aFilePath];

		return true;
	}

	Texture* ModelLoader::LoadTexture(const Graphics& aGfx, const std::string& aFilePath, const UINT aSlot)
	{
		if (textures.contains(aFilePath))
		{
			return &textures[aFilePath];
		}

		textures[aFilePath] = Texture(aSlot);
		textures[aFilePath].LoadTextureFromPath(aGfx, aFilePath);

		return &textures[aFilePath];
	}

	bool ModelLoader::LoadAnimation(AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath)
	{
		if (animationClips.contains(aFilePath))
		{
			aOutModelData.animationClipMap[animationClips[aFilePath].name] = &animationClips[aFilePath];
			aOutModelData.animationNames.push_back(animationClips[aFilePath].name);
			return true;
		}

		TGA::FBXAnimation animation;

		if (TGA::FBXImporter::LoadAnimation(aFilePath, aOutModelData.skeleton->boneNames, animation))
		{
			animationClips[aFilePath] = AnimationClip();
			AnimationClip& newAnimation = animationClips[aFilePath];

			newAnimation.name = animation.Name;

			// Trim path from name
			const size_t index = newAnimation.name.find_last_of('\\');
			newAnimation.name.erase(newAnimation.name.begin(), newAnimation.name.begin() + index + 1);

			newAnimation.length = animation.Length;
			newAnimation.fps = animation.FramesPerSecond;
			newAnimation.duration = (float)animation.Duration;
			newAnimation.keyframes.resize(animation.Frames.size());

			for (size_t f = 0; f < newAnimation.keyframes.size(); f++)
			{
				newAnimation.keyframes[f].boneTransforms.resize(animation.Frames[f].LocalTransforms.size());

				for (size_t t = 0; t < animation.Frames[f].LocalTransforms.size(); t++)
				{
					DirectX::XMMATRIX localMatrix = {};
					memcpy(&localMatrix, &animation.Frames[f].LocalTransforms[t], sizeof(float) * 16);

					newAnimation.keyframes[f].boneTransforms[t] = localMatrix;
				}
			}

			aOutModelData.animationClipMap[newAnimation.name] = &newAnimation;
			aOutModelData.animationNames.push_back(newAnimation.name);

			return true;
		}

		return false;
	}
}
