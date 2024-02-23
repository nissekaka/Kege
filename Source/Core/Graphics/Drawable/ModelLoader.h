#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
//#include <assimp/matrix4x4.h>
//#include <assimp/scene.h>

#include "ModelData.h"


//struct aiScene;

namespace Kaka
{
	struct Mesh;

	class ModelLoader
	{
	public:
		static bool LoadStaticModel(const Graphics& aGfx, const std::string& aFilePath, ModelDataPtr& aOutModelData);

		//static bool LoadModel(ModelData* aOutModelData, const std::string& aFilePath);
		static bool LoadModel(ModelDataPtr& aOutModelData, const std::string& aFilePath);
		static bool LoadAnimatedModel(AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath);
		static bool LoadTexture(const Graphics& aGfx, AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath);
		static bool LoadTexture(const Graphics& aGfx, ModelDataPtr& aOutModelData, const std::string& aFilePath);
		static Texture* LoadTexture(const Graphics& aGfx, const std::string& aFilePath, const UINT aSlot = 2u);
		static bool LoadAnimation(AnimatedModelDataPtr& aOutModelData, const std::string& aFilePath);

	private:
		//static Skeleton LoadSkeleton(const aiScene* aScene);
		//static std::vector<AnimationClip> LoadAnimations(const aiScene* aScene);
		//static DirectX::XMFLOAT4X4 AssimpToDirectXMatrix(const aiMatrix4x4& aAssimpMatrix);

		inline static std::unordered_map<std::string, MeshList> meshLists;
		inline static std::unordered_map<std::string, AnimatedMeshList> animatedMeshLists;
		inline static std::unordered_map<std::string, Skeleton> skeletons;
		inline static std::unordered_map<std::string, AnimationClip> animationClips;
		inline static std::unordered_map<std::string, Texture> textures;
	};
}
