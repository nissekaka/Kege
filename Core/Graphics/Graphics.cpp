#include "Graphics.h"
#include "Game/Source/Game.h"
#include <External/include/imgui/imgui_impl_dx11.h>
#include <External/include/imgui/imgui_impl_win32.h>

namespace WRL = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace Kaka
{
	Graphics::Graphics(HWND aHWnd, UINT aWidth, UINT aHeight)
		:
		width(aWidth), height(aHeight)
	{
		DXGI_SWAP_CHAIN_DESC scd = {};
		scd.BufferDesc.Width = width;
		scd.BufferDesc.Height = height;
		scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator = 0u;
		scd.BufferDesc.RefreshRate.Denominator = 0u;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.SampleDesc.Count = 1u; // Anti-aliasing
		scd.SampleDesc.Quality = 0u; // Anti-aliasing
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1u; // 1 back buffer and 1 front buffer
		scd.OutputWindow = aHWnd;
		scd.Windowed = true;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = 0u;

		UINT swapCreateFlags = 0u;
#ifndef NDEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapCreateFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&scd,
			&pSwap,
			&pDevice,
			nullptr,
			&pContext
		);

		// Gain access to texture subresource in swap chains (back buffer)
		WRL::ComPtr<ID3D11Resource> pBackBuffer;
		pSwap->GetBuffer(0u, __uuidof(ID3D11Resource), &pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

		// Create depth stencil state
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		WRL::ComPtr<ID3D11DepthStencilState> pDSState;
		pDevice->CreateDepthStencilState(&dsDesc, &pDSState);

		// Bind depth state
		pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

		// Create depth stencil texture
		WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

		// Create view of depth stencil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDepth);

		// Bind depth stencil view to OM
		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepth.Get());

		// Configure viewport
		D3D11_VIEWPORT vp = {};
		vp.Width = static_cast<FLOAT>(width);
		vp.Height = static_cast<FLOAT>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		pContext->RSSetViewports(1u, &vp);

		HRESULT result;

		ID3D11Texture2D* texture;
		// Reflection texture
		D3D11_TEXTURE2D_DESC desc = {0};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1u;
		desc.ArraySize = 1u;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0u;
		desc.MiscFlags = 0u;
		result = pDevice->CreateTexture2D(&desc, nullptr, &texture);
		assert(SUCCEEDED(result));
		result = pDevice->CreateShaderResourceView(texture, nullptr, &renderWaterReflect.pResource);
		assert(SUCCEEDED(result));
		result = pDevice->CreateRenderTargetView(texture, nullptr, &renderWaterReflect.pTarget);
		assert(SUCCEEDED(result));

		texture->Release();

		// Blend state
		D3D11_BLEND_DESC omDesc = {0};
		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = pDevice->CreateBlendState(&omDesc, &pBlend);
		assert(SUCCEEDED(result));

		// Init imgui d3d impl
		ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	}

	Graphics::~Graphics()
	{
		ImGui_ImplDX11_Shutdown();
	}

	void Graphics::BeginFrame()
	{
		// ImGui begin frame
		if (imGuiEnabled)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}

		constexpr float colour[] = KAKA_BG_COLOUR;
		pContext->ClearRenderTargetView(pTarget.Get(), colour);
		pContext->ClearDepthStencilView(pDepth.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void Graphics::EndFrame()
	{
		// ImGui end frame
		if (imGuiEnabled)
		{
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		if (HRESULT hr; FAILED(hr = pSwap->Present(0u, 0u)))
		{
			assert(hr == DXGI_ERROR_DEVICE_REMOVED && "Device removed");
		}

		drawcallCount = 0u;
	}

	void Graphics::DrawIndexed(const UINT aCount)
	{
		drawcallCount++;
		pContext->DrawIndexed(aCount, 0u, 0u);
	}

	void Graphics::SetProjection(DirectX::FXMMATRIX& aProjection)
	{
		projection = aProjection;
	}

	DirectX::XMMATRIX Graphics::GetProjection() const
	{
		return projection;
	}

	void Graphics::SetCamera(DirectX::FXMMATRIX& aCamera)
	{
		camera = aCamera;
	}

	DirectX::XMMATRIX Graphics::GetCamera() const
	{
		return DirectX::XMMatrixInverse(nullptr, camera);
	}

	UINT Graphics::GetDrawcallCount() const
	{
		return drawcallCount;
	}

	void Graphics::SetWaterReflectTarget()
	{
		constexpr float colour[] = KAKA_BG_COLOUR;

		pContext->OMSetRenderTargets(1u, renderWaterReflect.pTarget.GetAddressOf(), pDepth.Get());
		pContext->ClearRenderTargetView(renderWaterReflect.pTarget.Get(), colour);
		pContext->ClearDepthStencilView(pDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Graphics::SetDefaultTarget()
	{
		constexpr float colour[] = KAKA_BG_COLOUR;

		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepth.Get());
		pContext->ClearRenderTargetView(pTarget.Get(), colour);
		pContext->ClearDepthStencilView(pDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Graphics::SetAlpha() const
	{
		pContext->OMSetBlendState(pBlend.Get(), nullptr, 0x0f);
	}

	void Graphics::ResetAlpha() const
	{
		pContext->OMSetBlendState(nullptr, nullptr, 0x0f);
	}

	void Graphics::BindWaterReflectionTexture()
	{
		pContext->PSSetShaderResources(2u, 1u,
		                               renderWaterReflect.pResource.GetAddressOf());
	}

	DirectX::XMFLOAT2 Graphics::GetCurrentResolution() const
	{
		return {(float)width, (float)height};
	}

	void Graphics::EnableImGui()
	{
		imGuiEnabled = true;
	}

	void Graphics::DisableImGui()
	{
		imGuiEnabled = false;
	}

	bool Graphics::IsImGuiEnabled() const
	{
		return imGuiEnabled;
	}

	UINT Graphics::GetWidth() const
	{
		return width;
	}

	UINT Graphics::GetHeight() const
	{
		return height;
	}

	void Graphics::DrawTestTriangle2D()
	{
		struct Vertex
		{
			struct
			{
				float x;
				float y;
			} pos;

			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} colour;
		};

		const Vertex vertices[] = {
			{{0.0f, 1.0f}, {255, 0, 0, 255}},
			{{1.0f, -1.0f}, {0, 255, 0, 255}},
			{{-1.0f, -1.0f}, {0, 0, 255, 255}},
		};

		// Create vertex buffer
		WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC vbd = {};
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.CPUAccessFlags = 0u;
		vbd.MiscFlags = 0u;
		vbd.ByteWidth = sizeof(vertices);
		vbd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA vsd = {};
		vsd.pSysMem = vertices;
		pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer);

		// Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		// Create index buffer
		const unsigned short indices[] =
		{
			0, 1, 2,
		};

		WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

		// Bind index buffer
		pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		// Create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			DirectX::XMMATRIX transform;
		};
		const ConstantBuffer cb =
		{
			{

				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixRotationZ(0.0f) *
					DirectX::XMMatrixRotationX(0.0f) *
					DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f + 4.0f) *
					DirectX::XMMatrixPerspectiveLH(1.0f, static_cast<float>(height) / static_cast<float>(width), 0.5f,
					                               10.0f) *
					DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f)
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
		pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);

		// Bind constant buffer to vertex shader
		pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

		// Create pixel shader
		WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"Shaders\\3D_PS.cso", &pBlob);
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

		// Bind pixel shader
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		// Create vertex shader
		WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"Shaders\\3D_VS.cso", &pBlob);
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

		// Bind vertex shader
		pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		// Input (vertex) layout (2D position only)
		WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOUR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		pDevice->CreateInputLayout(
			ied,
			static_cast<UINT>(std::size(ied)),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout);

		// Bind vertex layout
		pContext->IASetInputLayout(pInputLayout.Get());

		// Set primitive topology to triangle list (groups of 3 vertices)
		pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
	}

	void Graphics::DrawTestCube3D(const float aAngle, const DirectX::XMFLOAT3 aPos)
	{
		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;

			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} colour;
		};

		const Vertex vertices[] = {
			{{-0.5f, -0.5f, -0.5f}, {255, 0, 0, 255}},
			{{0.5f, -0.5f, -0.5f}, {0, 255, 0, 255}},
			{{-0.5f, 0.5f, -0.5f}, {0, 0, 255, 255}},
			{{0.5f, 0.5f, -0.5f}, {255, 0, 0, 255}},
			{{-0.5f, -0.5f, 0.5f}, {0, 255, 0, 255}},
			{{0.5f, -0.5f, 0.5f}, {0, 0, 255, 255}},
			{{-0.5f, 0.5f, 0.5f}, {128, 128, 0, 255}},
			{{0.5f, 0.5f, 0.5f}, {0, 128, 128, 255}},
		};

		// Create vertex buffer
		WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC vbd = {};
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.CPUAccessFlags = 0u;
		vbd.MiscFlags = 0u;
		vbd.ByteWidth = sizeof(vertices);
		vbd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA vsd = {};
		vsd.pSysMem = vertices;
		pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer);

		// Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		// Create index buffer
		const unsigned short indices[] =
		{
			0, 2, 1, 2, 3, 1,
			1, 3, 5, 3, 7, 5,
			2, 6, 3, 3, 6, 7,
			4, 5, 7, 4, 7, 6,
			0, 4, 2, 2, 4, 6,
			0, 1, 4, 1, 5, 4
		};

		WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

		// Bind index buffer
		pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		// Create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			DirectX::XMMATRIX transform;
		};
		const ConstantBuffer cb =
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(aAngle) *
				DirectX::XMMatrixRotationX(aAngle) *
				DirectX::XMMatrixTranslation(aPos.x, aPos.y, aPos.z) *
				GetCamera() *
				GetProjection()
			)
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
		pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);

		// Bind constant buffer to vertex shader
		pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

		// Create pixel shader
		WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"Shaders\\3D_PS.cso", &pBlob);
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

		// Bind pixel shader
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		// Create vertex shader
		WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"Shaders\\3D_VS.cso", &pBlob);
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

		// Bind vertex shader
		pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		// Input (vertex) layout (2D position only)
		WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{"COLOUR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		pDevice->CreateInputLayout(
			ied,
			static_cast<UINT>(std::size(ied)),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout);

		// Bind vertex layout
		pContext->IASetInputLayout(pInputLayout.Get());

		// Set primitive topology to triangle list (groups of 3 vertices)
		pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
	}
}
