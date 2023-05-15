#pragma once
#include <string>

#include "Core/Model/Vertex.h"
#include <vector>

namespace Kaka
{
	enum class eModelType { None, Static, Skeletal };

	struct Bone
	{
		std::string name;
		DirectX::XMFLOAT4X4 offsetMatrix{};
	};

	struct Keyframe
	{
		float time = 0.0f;
		std::vector<DirectX::XMFLOAT4X4> boneTransforms{};
	};

	struct Skeleton
	{
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
		std::vector<AnimationClip> animations{};
		std::vector<DirectX::XMFLOAT4X4> defaultPose;
		eModelType modelType = eModelType::None;
	};
}
