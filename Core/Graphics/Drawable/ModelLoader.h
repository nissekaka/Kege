#pragma once
#include <string>
#include <iostream>
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
	private:
		static Skeleton LoadSkeleton(const aiScene* aScene);
		static std::vector<AnimationClip> LoadAnimations(const aiScene* aScene);
		static DirectX::XMFLOAT4X4 AssimpToDirectXMatrix(const aiMatrix4x4& aAssimpMatrix);
	};
}
