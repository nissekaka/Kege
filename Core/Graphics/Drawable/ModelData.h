#pragma once
#include <string>

#include "Core/Graphics/Drawable/Vertex.h"
#include <vector>

namespace Kaka
{
	enum class eModelType { None, Static, Skeletal };

	struct Bone
	{
		std::string name;
		DirectX::XMFLOAT4X4 offsetMatrix{};
		int parentIndex = -1;
		std::vector<int> childIndices;
	};

	struct Keyframe
	{
		float time = 0.0f;
		std::vector<DirectX::XMFLOAT4X4> boneTransforms{};
	};

	struct Skeleton
	{
		int rootBoneIndex = -1; // Index of the root bone in the skeleton
		std::vector<Bone> bones{};
	};

	struct AnimationClip
	{
		std::string name = "None";
		std::vector<Keyframe> keyframes{};
	};

	struct Mesh
	{
		std::vector<Vertex> vertices{};
		std::vector<unsigned short> indices{};
	};

	struct AnimatedMesh
	{
		std::vector<BoneVertex> vertices{};
		std::vector<unsigned short> indices{};
	};

	struct ModelData
	{
		ModelData() = default;
		Mesh mesh;
		AnimatedMesh animMesh;
		Skeleton skeleton;
		std::vector<AnimationClip> animations = {};
		std::vector<DirectX::XMFLOAT4X4> bindPose = {};
		eModelType modelType = eModelType::None;
		DirectX::XMFLOAT4X4 globalInverseMatrix = {};
	};
}
