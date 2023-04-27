#pragma once
#include "Model.h"
#include "Core/Model/Mesh.h"
#include "Core/Texture/Texture.h"
#include <DirectXMath.h>
#include <string>

namespace Kaka
{
	class Graphics;

	class Model
	{
	public:
		Model(const Graphics& aGfx, const std::string& aFilePath);
		~Model() = default;
		void Draw(const Graphics& aGfx);
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		void SetRotation(DirectX::XMFLOAT3 aRotation);
		void SetScale(float aScale);
		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMMATRIX GetTransform() const;
		float GetScale() const;
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
	private:
		struct TransformParameters
		{
			float roll = 0.0f;
			float pitch = 0.0f;
			float yaw = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			float scale = 1.0f;
		};

		TransformParameters transform;
	private:
		Mesh mesh;
		Texture texture;
	};
}
