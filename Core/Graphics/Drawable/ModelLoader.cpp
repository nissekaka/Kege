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
		                                         /*aiProcess_GenSmoothNormals |*/
		                                         aiProcess_FindInvalidData |
		                                         aiProcessPreset_TargetRealtime_Quality
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Failed to load file: " << importer.GetErrorString() << std::endl;
			return false;
		}

		const aiMesh* mesh = scene->mMeshes[0];

		aOutModelData.globalInverseMatrix = AssimpToDirectXMatrix(scene->mRootNode->mTransformation);
		DirectX::XMStoreFloat4x4(&aOutModelData.globalInverseMatrix,
		                         DirectX::XMMatrixInverse(
			                         nullptr, DirectX::XMLoadFloat4x4(&aOutModelData.globalInverseMatrix)));

		// Check if scene contains animations
		if (scene->mNumAnimations > 0)
		{
			// Load the skeleton and animations
			aOutModelData.skeleton = LoadSkeleton(scene);
			aOutModelData.animations = LoadAnimations(scene);

			for (const Bone& bone : aOutModelData.skeleton.bones)
			{
				// Compute the bind pose transformation matrix as the inverse of the bone's offset matrix
				DirectX::XMFLOAT4X4 bindPoseBoneTransform;
				DirectX::XMStoreFloat4x4(&bindPoseBoneTransform,
				                         DirectX::XMMatrixInverse(
					                         nullptr, DirectX::XMLoadFloat4x4(&bone.offsetMatrix)));

				// Store the bind pose transformation matrix
				aOutModelData.bindPose.push_back(bindPoseBoneTransform);
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
				std::array<unsigned int, 4> boneIndices = {};
				std::array<float, 4> boneWeights = {};

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
					// Keep track of the number of bone influences encountered
					unsigned int numInfluences = 0;

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
								// Find the index for the bone in the boneIndices vector
								auto it = std::find(boneIndices.begin(), boneIndices.end(), j);
								if (it == boneIndices.end())
								{
									// The bone index is not already in the boneIndices vector, add it
									if (numInfluences < boneIndices.size())
									{
										boneIndices[numInfluences] = j;
										boneWeights[numInfluences] = vertexWeight.mWeight;
										numInfluences++;
									}
								}
								else
								{
									// The bone index already exists in the boneIndices vector, update its weight
									size_t index = std::distance(boneIndices.begin(), it);
									boneWeights[index] = vertexWeight.mWeight;
								}
							}
						}
					}

					// Normalize bone weights if there are multiple influences
					if (numInfluences > 1)
					{
						// Calculate the sum of bone weights
						float weightSum = 0.0f;
						for (unsigned int l = 0; l < numInfluences; ++l)
						{
							weightSum += boneWeights[l];
						}

						// Normalize the bone weights
						for (unsigned int l = 0; l < numInfluences; ++l)
						{
							boneWeights[l] /= weightSum;
						}
					}
				}

				aOutModelData.animMesh.vertices.push_back({
					position,normal,texCoord,tangent,bitangent,{boneIndices[0],boneIndices[1],boneIndices[2],boneIndices[3]},{boneWeights[0],boneWeights[1],boneWeights[2],boneWeights[3]}
				});

				//for (int k = 0; k < 4; ++k)
				//{
				//	std::string boneInfo = "\n Vertex: " + std::to_string(i) + " [" + std::to_string(k) + "] Bone: " + std::to_string(boneIndices[k]) + " Weight: " + std::to_string(boneWeights[k]);
				//	OutputDebugStringA(boneInfo.c_str());
				//}
				//OutputDebugStringA("\n---");
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

		for (unsigned int i = 0; i < aScene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = aScene->mMeshes[i];

			for (unsigned int j = 0; j < mesh->mNumBones; ++j)
			{
				const aiBone* bone = mesh->mBones[j];

				Bone skeletonBone;
				skeletonBone.name = bone->mName.C_Str();
				skeletonBone.offsetMatrix = AssimpToDirectXMatrix(bone->mOffsetMatrix);

				skeleton.bones.push_back(skeletonBone);
			}
		}

		for (unsigned int i = 0; i < skeleton.bones.size(); ++i)
		{
			const aiNode* boneNode = aScene->mRootNode->FindNode(skeleton.bones[i].name.c_str());
			if (boneNode)
			{
				const aiNode* parentNode = boneNode->mParent;
				if (parentNode)
				{
					std::string parentName = parentNode->mName.C_Str();
					for (unsigned int j = 0; j < skeleton.bones.size(); ++j)
					{
						if (skeleton.bones[j].name == parentName)
						{
							skeleton.bones[i].parentIndex = j;
							skeleton.bones[j].childIndices.push_back(i);
							break;
						}
					}
				}
			}
		}

		// Find and set the rootBoneIndex
		for (int i = 0; i < skeleton.bones.size(); ++i)
		{
			if (skeleton.bones[i].parentIndex == -1)
			{
				skeleton.rootBoneIndex = i;
				break;
			}
		}

		return skeleton;
	}

	// Helper function to find the index of the keyframe that corresponds to the given time
	unsigned int FindKeyframeIndex(const aiNodeAnim* aChannel, const float aTime)
	{
		const unsigned int numKeys = aChannel->mNumPositionKeys;

		// Handle cases where the given time is greater than the last keyframe time
		if (aTime >= aChannel->mPositionKeys[numKeys - 1].mTime)
		{
			return numKeys - 1; // Return the index of the last keyframe
		}

		// Iterate through the position keys to find the index of the keyframe that matches or is closest to the given time
		for (unsigned int i = 0; i < numKeys - 1; ++i)
		{
			if (aTime < aChannel->mPositionKeys[i + 1].mTime)
			{
				return i; // Return the index of the keyframe
			}
		}

		// If no keyframe was found, return the last keyframe index
		return numKeys - 1;
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

			const float animationTicksPerSecond = animation->mTicksPerSecond != 0 ? static_cast<float>(animation->mTicksPerSecond) : 25.0f;
			const float animationDuration = static_cast<float>(animation->mDuration) / animationTicksPerSecond;

			// Determine the maximum number of frames among all the channels
			unsigned int numFrames = 0;
			for (unsigned int j = 1; j < animation->mNumChannels; ++j)
			{
				const aiNodeAnim* channel = animation->mChannels[j];
				numFrames = std::max(numFrames, channel->mNumPositionKeys);
			}

			// Calculate the duration of a single frame in seconds
			float frameDuration = animationDuration / static_cast<float>(numFrames);

			// Iterate through each frame
			for (unsigned int frameIndex = 0; frameIndex < numFrames; ++frameIndex)
			{
				// Create a keyframe for the current frame
				Keyframe keyframe;
				keyframe.time = (float)frameIndex * frameDuration;

				// Iterate through each channel (bone)
				for (unsigned int j = 1; j < animation->mNumChannels; ++j)
				{
					const aiNodeAnim* channel = animation->mChannels[j];

					//const unsigned int keyframeIndex = FindKeyframeIndex(channel, keyframe.time);
					const aiVectorKey& positionKey = channel->mPositionKeys[frameIndex];
					//const aiQuatKey& rotationKey = channel->mRotationKeys[frameIndex];
					//const aiVectorKey& scalingKey = channel->mScalingKeys[frameIndex];

					// Create the bone transform matrix from the keyframe data
					DirectX::XMFLOAT4X4 boneTransform{};
					DirectX::XMStoreFloat4x4(&boneTransform, DirectX::XMMatrixIdentity());

					DirectX::XMFLOAT3 position(positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z);
					std::string posKey = "\nIndex: " + std::to_string(frameIndex) + "  Pos: " + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z);
					OutputDebugStringA(posKey.c_str());
					//DirectX::XMFLOAT4 rotation(rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z, rotationKey.mValue.w);
					//DirectX::XMFLOAT3 scaling(scalingKey.mValue.x, scalingKey.mValue.y, scalingKey.mValue.z);
					DirectX::XMFLOAT4 rotation(0.0f, 0.0f, 0.0f, 0.0f);
					DirectX::XMFLOAT3 scaling(1.0f, 1.0f, 1.0f);

					DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
					DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
					DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scaling));

					DirectX::XMMATRIX boneTransformMatrix = translationMatrix * rotationMatrix * scalingMatrix;
					DirectX::XMStoreFloat4x4(&boneTransform, boneTransformMatrix);

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

	DirectX::XMFLOAT4X4 ModelLoader::AssimpToDirectXMatrix(const aiMatrix4x4& aAssimpMatrix)
	{
		return {
			aAssimpMatrix.a1,aAssimpMatrix.b1,aAssimpMatrix.c1,aAssimpMatrix.d1,
			aAssimpMatrix.a2,aAssimpMatrix.b2,aAssimpMatrix.c2,aAssimpMatrix.d2,
			aAssimpMatrix.a3,aAssimpMatrix.b3,aAssimpMatrix.c3,aAssimpMatrix.d3,
			aAssimpMatrix.a4,aAssimpMatrix.b4,aAssimpMatrix.c4,aAssimpMatrix.d4
		};
	}

	//bool Mesh::LoadMesh(const std::string& Filename)
	//{
	//	m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
	//		aiProcess_FlipUVs);

	//	if (m_pScene)
	//	{
	//		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
	//		m_GlobalInverseTransform = m_GlobalInverseTransform.Inverse();
	//	}
	//	else
	//	{
	//		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	//	}

	//	return Ret;
	//}

	//void Mesh::LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<BoneVertex>& Bones)
	//{
	//	for (uint i = 0; i < pMesh->mNumBones; i++)
	//	{
	//		uint BoneIndex = 0;
	//		string BoneName(pMesh->mBones[i]->mName.data);

	//		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
	//		{
	//			BoneIndex = m_NumBones;
	//			m_NumBones++;
	//			BoneInfo bi;
	//			m_BoneInfo.push_back(bi);
	//		}
	//		else
	//		{
	//			BoneIndex = m_BoneMapping[BoneName];
	//		}

	//		m_BoneMapping[BoneName] = BoneIndex;
	//		m_BoneInfo[BoneIndex].BoneOffset = AssimpToDirectXMatrix(pMesh->mBones[i]->mOffsetMatrix);

	//		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
	//		{
	//			uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
	//			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
	//			Bones[VertexID].AddBoneData(BoneIndex, Weight);
	//		}
	//	}
	//}

	//void Mesh::VertexBoneData::AddBoneData(uint BoneID, float Weight)
	//{
	//	for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++)
	//	{
	//		if (Weights[i] == 0.0f)
	//		{
	//			IDs[i] = BoneID;
	//			Weights[i] = Weight;
	//			return;
	//		}
	//	}

	//	// Should never get here - more bones than we have space for
	//	assert(0);
	//}

	//XMMATRIX Mesh::BoneTransform(float TimeInSeconds, vector<XMMATRIX>& Transforms)
	//{
	//	XMMATRIX Identity = XMMatrixIdentity();

	//	float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ?
	//		m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
	//	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	//	float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[0]->mDuration);

	//	ReadNodeHierarchy(AnimationTime, m_pScene->mRootNode, Identity);

	//	Transforms.resize(m_NumBones);

	//	for (uint i = 0; i < m_NumBones; i++)
	//	{
	//		Transforms[i] = XMMatrixTranspose(XMLoadFloat4x4(&m_BoneInfo[i].FinalTransformation));
	//	}
	//}

	//void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const XMMATRIX& ParentTransform)
	//{
	//	string NodeName(pNode->mName.data);

	//	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	//	XMMATRIX NodeTransformation = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&pNode->mTransformation)));

	//	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	//	if (pNodeAnim)
	//	{
	//		// Interpolate scaling and generate scaling transformation matrix
	//		aiVector3D Scaling;
	//		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
	//		XMMATRIX ScalingM = XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z);

	//		// Interpolate rotation and generate rotation transformation matrix
	//		aiQuaternion RotationQ;
	//		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
	//		XMMATRIX RotationM = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&RotationQ.GetMatrix())));

	//		// Interpolate translation and generate translation transformation matrix
	//		aiVector3D Translation;
	//		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
	//		XMMATRIX TranslationM = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);

	//		// Combine the above transformations
	//		NodeTransformation = TranslationM * RotationM * ScalingM;
	//	}

	//	XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;

	//	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
	//	{
	//		uint BoneIndex = m_BoneMapping[NodeName];
	//		XMMATRIX BoneOffset = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&m_BoneInfo[BoneIndex].BoneOffset)));
	//		m_BoneInfo[BoneIndex].FinalTransformation = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&m_GlobalInverseTransform))) * GlobalTransformation * BoneOffset;
	//	}

	//	for (uint i = 0; i < pNode->mNumChildren; i++)
	//	{
	//		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	//	}
	//}

	//void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	//{
	//	// We need at least two values to interpolate...
	//	if (pNodeAnim->mNumRotationKeys == 1)
	//	{
	//		Out = pNodeAnim->mRotationKeys[0].mValue;
	//		return;
	//	}

	//	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	//	uint NextRotationIndex = (RotationIndex + 1);
	//	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	//	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	//	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//	assert(Factor >= 0.0f && Factor <= 1.0f);
	//	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	//	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	//	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	//	Out = Out.Normalize();
	//}

	//uint Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	//{
	//	assert(pNodeAnim->mNumRotationKeys > 0);

	//	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	//	{
	//		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
	//		{
	//			return i;
	//		}
	//	}

	//	assert(0);
	//}
}