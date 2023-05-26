#include "Skybox.h"

namespace Kaka
{
	void Skybox::Init(const Graphics& aGfx, const std::string& aDayPath, const std::string& aNightPath)
	{
		UNREFERENCED_PARAMETER(aGfx);

		dayTexture.LoadTextures(aGfx, aDayPath);
		nightTexture.LoadTextures(aGfx, aNightPath);

		sampler.Init(aGfx, 0u);

		vertexBuffer.Init(aGfx, vertices);
		indexBuffer.Init(aGfx, indices);

		pixelShader.Init(aGfx, L"Shaders\\Skybox_PS.cso");
		vertexShader.Init(aGfx, L"Shaders\\Skybox_VS.cso");

		inputLayout.Init(aGfx, ied, vertexShader.GetBytecode());
		topology.Init(aGfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		rasterizer.Init(aGfx, true);
		depthStencil.Init(aGfx, DepthStencil::Mode::DepthFirst);
	}

	void Skybox::Draw(const Graphics& aGfx)
	{
		UNREFERENCED_PARAMETER(aGfx);

		sampler.Bind(aGfx);

		dayTexture.Bind(aGfx);
		nightTexture.Bind(aGfx);

		vertexBuffer.Bind(aGfx);
		indexBuffer.Bind(aGfx);

		SkyboxTransformConstantBuffer skyboxTransformConstantBuffer(aGfx, *this, 0u);
		skyboxTransformConstantBuffer.Bind(aGfx);

		pixelShader.Bind(aGfx);
		vertexShader.Bind(aGfx);

		inputLayout.Bind(aGfx);
		topology.Bind(aGfx);
		rasterizer.Bind(aGfx);
		depthStencil.Bind(aGfx);

		aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
		// Unbind shader resources
		ID3D11ShaderResourceView* nullSRVs[1] = {nullptr};
		aGfx.pContext->PSSetShaderResources(0, 1, nullSRVs);
		aGfx.pContext->PSSetShaderResources(1, 1, nullSRVs);
	}

	void Skybox::Rotate(const DirectX::XMFLOAT3 aRotation)
	{
		transform.roll = aRotation.x;
		transform.pitch = aRotation.y;
		transform.yaw = aRotation.z;
	}

	DirectX::XMMATRIX Skybox::GetTransform() const
	{
		return DirectX::XMMatrixScaling(transform.scale, transform.scale, transform.scale) *
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.roll) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
}
