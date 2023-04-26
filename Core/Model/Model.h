#pragma once
#include "Model.h"
#include "Core/Model/Mesh.h"
#include <DirectXMath.h>
#include <string>

namespace Kaka
{
	class Graphics;

	class Model
	{
	public:
		Model(const std::string& aFilePath);
		~Model() = default;
		void Draw(const Graphics& aGfx) const;
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		void SetRotation(DirectX::XMFLOAT3 aRotation);
		void SetScale(float aScale);
		DirectX::XMMATRIX GetTransform() const;
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
	};
}