#include "Model.h"
#include "Core/Graphics/Graphics.h"
#include "Core/Model/FBXLoader.h"
#include <d3dcompiler.h>
#include <wrl/client.h>

namespace Kaka
{
	Model::Model(const std::string& aFilePath)
	{
		angle = 0.0f;
		position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		FBXLoader::LoadMesh(aFilePath, mesh);
	}

	void Model::Rotate(const float aAngle)
	{
		angle = aAngle;
	}

	void Model::Draw(const Graphics& aGfx) const
	{
		namespace WRL = Microsoft::WRL;

		// Create vertex buffer
		WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC vbd = {};
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.CPUAccessFlags = 0u;
		vbd.MiscFlags = 0u;
		vbd.ByteWidth = sizeof(Vertex) * static_cast<UINT>(mesh.vertices.size());
		vbd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA vsd = {};
		vsd.pSysMem = &mesh.vertices[0];
		aGfx.pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer);

		// Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		aGfx.pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(unsigned short) * static_cast<UINT>(mesh.indices.size());
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = &mesh.indices[0];
		aGfx.pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

		// Bind index buffer
		aGfx.pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		// Create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			DirectX::XMMATRIX transform;
		};
		const ConstantBuffer cb =
		{
			{

				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixRotationZ(angle) *
					DirectX::XMMatrixRotationX(angle) *
					DirectX::XMMatrixTranslation(position.x, position.y, position.z + 4.0f) *
					DirectX::XMMatrixPerspectiveLH(1.0f, static_cast<float>(aGfx.GetHeight()) / static_cast<float>(aGfx.GetWidth()), 0.5f,
					                               10.0f)
				)
			}
		};
		WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cb);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &cb;
		aGfx.pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);

		// Bind constant buffer to vertex shader
		aGfx.pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

		// Create pixel shader
		WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"Shaders\\Default_PS.cso", &pBlob);
		aGfx.pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

		// Bind pixel shader
		aGfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		// Create vertex shader
		WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"Shaders\\3D_VS.cso", &pBlob);
		aGfx.pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

		// Bind vertex shader
		aGfx.pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		// Input (vertex) layout
		WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{
				"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},
			{
				"COLOUR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,
				D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0
			},

		};
		aGfx.pDevice->CreateInputLayout(
			ied,
			static_cast<UINT>(std::size(ied)),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout);

		// Bind vertex layour
		aGfx.pContext->IASetInputLayout(pInputLayout.Get());

		// Set primitive topology to triangle list (groups of 3 vertices)
		aGfx.pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Configure viewport
		D3D11_VIEWPORT vp = {};
		vp.Width = static_cast<FLOAT>(aGfx.GetWidth());
		vp.Height = static_cast<FLOAT>(aGfx.GetHeight());
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		aGfx.pContext->RSSetViewports(1u, &vp);

		aGfx.pContext->DrawIndexed(static_cast<UINT>(std::size(mesh.indices)), 0u, 0u);
	}

	void Model::SetPosition(const DirectX::XMFLOAT3 aPosition)
	{
		position = aPosition;
	}
}