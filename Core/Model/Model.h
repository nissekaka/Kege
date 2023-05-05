#pragma once
#include "Drawable.h"
#include "Core/Model/Mesh.h"
#include "Core/Graphics/Bindable/BindableCommon.h"
#include <DirectXMath.h>
#include <string>


namespace Kaka
{
	class Graphics;

	class Model : public Drawable
	{
	public:
		enum class eShaderType { Default, Light, Phong };

	public:
		Model(const Graphics& aGfx, const std::string& aFilePath, const eShaderType aShaderType);
		~Model() override = default;
		void Draw(const Graphics& aGfx);
		void SetPosition(DirectX::XMFLOAT3 aPosition);
		void SetRotation(DirectX::XMFLOAT3 aRotation);
		void SetScale(float aScale);
		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMMATRIX GetTransform() const override;
		float GetScale() const;
	public:
		void ShowControlWindow(const char* aWindowName = nullptr);
	private:
		eShaderType shaderType;
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
		Texture texture{0u};
		std::vector<std::unique_ptr<Bindable>> bindablePtrs;
	};
}
