#pragma once
#include "DirectXMath.h"
#include <vector>

namespace Kaka
{
	class Texture;

	enum class eSpriteMode
	{
		Default,
		Billboard,
		Screen
	};

	struct SpriteVertex
	{
		float x, y, z, w;
		float u, v;
		float padding[2];
	};

	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;
		float scale = 1.0f;
	};

	struct Sprite
	{
		TransformParameters transform;
		DirectX::XMFLOAT4 colour = {1.0f, 1.0f, 1.0f, 1.0f};
		DirectX::XMFLOAT4 uv = {0.0f, 0.0f, 1.0f, 1.0f};
	};

	struct SpriteData
	{
		Texture* texture = nullptr;
	};

	struct SpriteBatch
	{
		SpriteData data;
		std::vector<Sprite> instances{};
		eSpriteMode mode = eSpriteMode::Default;
	};
}
