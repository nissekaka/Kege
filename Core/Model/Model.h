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
		void Rotate(float aAngle);
		void Draw(const Graphics& aGfx) const;
		void SetPosition(DirectX::XMFLOAT3 aPosition);
	private:
		float angle;
		DirectX::XMFLOAT3 position;

		Mesh mesh;
	};
}