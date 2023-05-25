#include "Skybox.h"

namespace Kaka
{
	void Skybox::Init(const Graphics& aGfx)
	{
		UNREFERENCED_PARAMETER(aGfx);

		texture.LoadTextures(aGfx, "Assets\\Textures\\Skybox\\");

		sampler.Init(aGfx, 0u);

		vertexBuffer.Init(aGfx, vertices);
		indexBuffer.Init(aGfx, indices);

		pixelShader.Init(aGfx, L"Shaders\\Skybox_PS.cso");
		vertexShader.Init(aGfx, L"Shaders\\Skybox_VS.cso");

		inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
		topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Skybox::Draw(const Graphics& aGfx)
	{
		UNREFERENCED_PARAMETER(aGfx);

		sampler.Bind(aGfx);

		texture.Bind(aGfx);

		vertexBuffer.Bind(aGfx);
		indexBuffer.Bind(aGfx);

		//TransformConstantBuffer transformConstantBuffer(aGfx, *this, 0u);
		//transformConstantBuffer.Bind(aGfx);

		pixelShader.Bind(aGfx);
		vertexShader.Bind(aGfx);

		inputLayout.Bind(aGfx);

		topology.Bind(aGfx);

		aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
	}

	DirectX::XMMATRIX Skybox::GetTransform() const
	{
		return DirectX::XMMatrixScaling(transform.scale, transform.scale, transform.scale) *
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.roll) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
}