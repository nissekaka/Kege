#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

#include "ModelData.h"


struct aiScene;

namespace Kaka
{
	struct Mesh;

	class ModelLoader
	{
	public:
		static bool LoadModel(const std::string& aFilePath, ModelData& aOutModelData);

		static bool LoadAnimatedModel(AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath);
		static bool LoadAnimation(AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath);

	private:
		static Skeleton LoadSkeleton(const aiScene* aScene);
		static std::vector<AnimationClip> LoadAnimations(const aiScene* aScene);
		static DirectX::XMFLOAT4X4 AssimpToDirectXMatrix(const aiMatrix4x4& aAssimpMatrix);

		inline static std::unordered_map<std::string, MeshList> meshLists;
		inline static std::unordered_map<std::string, AnimatedMeshList> animatedMeshLists;
		inline static std::unordered_map<std::string, Skeleton> skeletons;
		inline static std::unordered_map<std::string, AnimationClip> animationClips;
	};
}
