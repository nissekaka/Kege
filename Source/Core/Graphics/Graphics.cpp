#include "stdafx.h"
#include "Graphics.h"
#include "Game/Source/Game.h"
#include <External/include/imgui/imgui_impl_dx11.h>
#include <External/include/imgui/imgui_impl_win32.h>
#include <complex>

#include "Drawable/ModelLoader.h"
#include "Shaders/ShaderFactory.h"

namespace WRL = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace Kaka
{
	Graphics::Graphics(HWND aHWnd, UINT aWidth, UINT aHeight)
		:
		width(aWidth),
		height(aHeight)
	{
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
			pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pDefaultTarget);
		}

		// Create depth stencil state
		{
			D3D11_TEXTURE2D_DESC desc = {0};
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			ID3D11Texture2D* texture;
			HRESULT result = pDevice->CreateTexture2D(&desc, nullptr, &texture);
			assert(SUCCEEDED(result));

			ID3D11DepthStencilView* DSV;
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

			dsvDesc.Flags = 0;
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			result = pDevice->CreateDepthStencilView(texture, &dsvDesc, &DSV);
			assert(SUCCEEDED(result));

			pDepth = DSV;
			DSV->Release();

			ID3D11ShaderResourceView* SRV;
			D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
			srDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srDesc.Texture2D.MostDetailedMip = 0;
			srDesc.Texture2D.MipLevels = UINT_MAX;
			result = pDevice->CreateShaderResourceView(texture, &srDesc, &SRV);
			assert(SUCCEEDED(result));
			pDepthShaderResourceView = SRV;
			SRV->Release();

			//D3D11_DEPTH_STENCIL_DESC dsDesc = {};
			//dsDesc.DepthEnable = TRUE;
			//dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			//dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
			//WRL::ComPtr<ID3D11DepthStencilState> pDSState;
			//pDevice->CreateDepthStencilState(&dsDesc, &pDSState);

			//// Bind depth state
			//pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

			//// Create depth stencil texture
			//WRL::ComPtr<ID3D11Texture2D> depthTexture;
			//D3D11_TEXTURE2D_DESC descDepth = {};
			//descDepth.Width = width;
			//descDepth.Height = height;
			//descDepth.MipLevels = 1u;
			//descDepth.ArraySize = 1u;
			//descDepth.Format = DXGI_FORMAT_D32_FLOAT;
			//descDepth.SampleDesc.Count = 1u;
			//descDepth.SampleDesc.Quality = 0u;
			//descDepth.Usage = D3D11_USAGE_DEFAULT;
			//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			//pDevice->CreateTexture2D(&descDepth, nullptr, &depthTexture);

			//// Create view of depth stencil texture
			//D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
			//descDSV.Format = DXGI_FORMAT_D32_FLOAT;
			//descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			//descDSV.Texture2D.MipSlice = 0u;
			//pDevice->CreateDepthStencilView(depthTexture.Get(), &descDSV, &pDepth);

			//// I want to see the depth as a texture
			//D3D11_TEXTURE2D_DESC desc = {0};
			//desc.Width = width;
			//desc.Height = height;
			//desc.MipLevels = 1;
			//desc.ArraySize = 1;
			//desc.Format = DXGI_FORMAT_R32_TYPELESS;
			//desc.SampleDesc.Count = 1;
			//desc.SampleDesc.Quality = 0;
			//desc.Usage = D3D11_USAGE_DEFAULT;
			//desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			//desc.CPUAccessFlags = 0;
			//desc.MiscFlags = 0;

			//ID3D11Texture2D* texture;
			//HRESULT result = pDevice->CreateTexture2D(&desc, nullptr, &texture);
			//assert(SUCCEEDED(result));

			//ID3D11ShaderResourceView* SRV;
			//D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
			//srDesc.Format = DXGI_FORMAT_R32_FLOAT;
			//srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			//srDesc.Texture2D.MostDetailedMip = 0;
			//srDesc.Texture2D.MipLevels = UINT_MAX;
			//result = pDevice->CreateShaderResourceView(texture, &srDesc, &SRV);
			//assert(SUCCEEDED(result));
			//pDepthShaderResourceView = SRV;
			//SRV->Release();

			// Bind depth stencil view to OM
			pContext->OMSetRenderTargets(1u, pDefaultTarget.GetAddressOf(), pDepth.Get());
			//depthTexture->Release();

			// Configure viewport
			D3D11_VIEWPORT vp = {};
			vp.Width = static_cast<FLOAT>(width);
			vp.Height = static_cast<FLOAT>(height);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
			pContext->RSSetViewports(1u, &vp);
		}

		HRESULT result;

		// Reflection texture
		{
			ID3D11Texture2D* reflectionTexture;
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
			result = pDevice->CreateTexture2D(&desc, nullptr, &reflectionTexture);
			assert(SUCCEEDED(result));
			result = pDevice->CreateShaderResourceView(reflectionTexture, nullptr, &renderWaterReflect.pResource);
			assert(SUCCEEDED(result));
			result = pDevice->CreateRenderTargetView(reflectionTexture, nullptr, &renderWaterReflect.pTarget);
			assert(SUCCEEDED(result));

			reflectionTexture->Release();
		}

		// Post processing
		{
			ID3D11Texture2D* postTexture;
			D3D11_TEXTURE2D_DESC ppDesc = {0};
			ppDesc.Width = width;
			ppDesc.Height = height;
			ppDesc.MipLevels = 1u;
			ppDesc.ArraySize = 1u;
			ppDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			ppDesc.SampleDesc.Count = 1u;
			ppDesc.SampleDesc.Quality = 0u;
			ppDesc.Usage = D3D11_USAGE_DEFAULT;
			ppDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			ppDesc.CPUAccessFlags = 0u;
			ppDesc.MiscFlags = 0u;
			result = pDevice->CreateTexture2D(&ppDesc, nullptr, &postTexture);
			assert(SUCCEEDED(result));
			result = pDevice->CreateShaderResourceView(postTexture, nullptr, &postProcessing.pResource);
			assert(SUCCEEDED(result));
			result = pDevice->CreateRenderTargetView(postTexture, nullptr, &postProcessing.pTarget);
			assert(SUCCEEDED(result));

			postTexture->Release();
		}

		// Bloom
		{
			UINT bloomWidth = width;
			UINT bloomHeight = height;

			for (int i = 0; i < bloomSteps; ++i)
			{
				ID3D11Texture2D* bloomTexture;
				bloomWidth /= bloomDivideFactor;
				bloomHeight /= bloomDivideFactor;
				bloomDownscale.emplace_back();

				D3D11_TEXTURE2D_DESC ppDesc = {0};
				ppDesc.Width = bloomWidth;
				ppDesc.Height = bloomHeight;
				ppDesc.MipLevels = 1u;
				ppDesc.ArraySize = 1u;
				ppDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				ppDesc.SampleDesc.Count = 1u;
				ppDesc.SampleDesc.Quality = 0u;
				ppDesc.Usage = D3D11_USAGE_DEFAULT;
				ppDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				ppDesc.CPUAccessFlags = 0u;
				ppDesc.MiscFlags = 0u;
				result = pDevice->CreateTexture2D(&ppDesc, nullptr, &bloomTexture);
				assert(SUCCEEDED(result));
				result = pDevice->CreateShaderResourceView(bloomTexture, nullptr, &bloomDownscale.back().pResource);
				assert(SUCCEEDED(result));
				result = pDevice->CreateRenderTargetView(bloomTexture, nullptr, &bloomDownscale.back().pTarget);
				assert(SUCCEEDED(result));
				bloomTexture->Release();
			}
		}

		// Shadow
		{
			UINT shadowWidth = 1024.0f;
			UINT shadowHeight = 1024.0f;

			D3D11_TEXTURE2D_DESC desc = {0};
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			ID3D11Texture2D* texture;
			result = pDevice->CreateTexture2D(&desc, nullptr, &texture);
			assert(SUCCEEDED(result));

			ID3D11DepthStencilView* DSV;
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

			dsvDesc.Flags = 0;
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			result = pDevice->CreateDepthStencilView(texture, &dsvDesc, &DSV);
			assert(SUCCEEDED(result));

			pShadowDepth = DSV;
			DSV->Release();

			ID3D11ShaderResourceView* SRV;
			D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
			srDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srDesc.Texture2D.MostDetailedMip = 0;
			srDesc.Texture2D.MipLevels = UINT_MAX;
			result = pDevice->CreateShaderResourceView(texture, &srDesc, &SRV);
			assert(SUCCEEDED(result));
			shadowMap.pResource = SRV;
			SRV->Release();

			D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{CD3D11_DEFAULT{}};
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

			pDevice->CreateSamplerState(&samplerDesc, &pShadowSampler);

			D3D11_SAMPLER_DESC samplerCompDesc = CD3D11_SAMPLER_DESC{CD3D11_DEFAULT{}};
			samplerCompDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
			samplerCompDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerCompDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerCompDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerCompDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

			pDevice->CreateSamplerState(&samplerCompDesc, &pShadowCompSampler);
		}

		// World Position
		{
			ID3D11Texture2D* worldPosTexture;
			D3D11_TEXTURE2D_DESC desc = {0};
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			result = pDevice->CreateTexture2D(&desc, nullptr, &worldPosTexture);
			assert(SUCCEEDED(result));
			result = pDevice->CreateShaderResourceView(worldPosTexture, nullptr, &worldPosition.pResource);
			assert(SUCCEEDED(result));
			result = pDevice->CreateRenderTargetView(worldPosTexture, nullptr, &worldPosition.pTarget);
			assert(SUCCEEDED(result));

			worldPosTexture->Release();
		}

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

		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = pDevice->CreateBlendState(&omDesc, &pBlendVfx);
		assert(SUCCEEDED(result));

		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = pDevice->CreateBlendState(&omDesc, &pBlendAdd);
		assert(SUCCEEDED(result));

		// Init imgui d3d impl
		ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());

		// VFX
		{
			D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{CD3D11_DEFAULT{}};
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

			pDevice->CreateSamplerState(&samplerDesc, &pVFXSampler);

			pContext->PSSetSamplers(7u, 1u, pVFXSampler.GetAddressOf());
		}
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
		pContext->ClearRenderTargetView(pDefaultTarget.Get(), colour);
		pContext->ClearRenderTargetView(worldPosition.pTarget.Get(), colour);
		//pContext->ClearRenderTargetView(renderWaterReflect.pTarget.Get(), colour);
		//pContext->ClearRenderTargetView(postProcessing.pTarget.Get(), colour);
		//for (int i = 0; i < bloomSteps; ++i)
		//{
		//	pContext->ClearRenderTargetView(bloomDownscale[i].pTarget.Get(), colour);
		//}
		pContext->ClearDepthStencilView(pDepth.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void Graphics::EndFrame()
	{
		//if (ImGui::Begin("the!!"))
		//{
		//	ImGui::Image(renderWaterReflect.pResource.Get(), ImVec2(1600 / 4, 800 / 4));
		//}
		//ImGui::End();

		if (ImGui::Begin("Bloom"))
		{
			ImGui::Checkbox("Use bloom", &useBloom);
			ImGui::SetNextItemWidth(100);
			ImGui::SliderFloat("Bloom blending", &bb.bloomBlending, 0.0f, 1.0f);
			ImGui::SetNextItemWidth(100);
			ImGui::SliderFloat("Bloom threshold", &bb.bloomThreshold, 0.0f, 1.0f);
		}
		ImGui::End();

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

	void Graphics::DrawIndexedInstanced(const UINT aCount, const UINT aInstanceCount)
	{
		drawcallCount++;
		pContext->DrawIndexedInstanced(aCount, aInstanceCount, 0u, 0u, 0u);
	}

	//void Graphics::SetProjection(DirectX::FXMMATRIX& aProjection)
	//{
	//	camera->projection = aProjection;
	//}

	DirectX::XMMATRIX Graphics::GetProjection() const
	{
		return camera->GetProjection();
	}

	void Graphics::SetCamera(Camera& aCamera)
	{
		camera = &aCamera;
	}

	DirectX::XMMATRIX Graphics::GetCameraInverseMatrix() const
	{
		return camera->GetInverseMatrix();
	}

	UINT Graphics::GetDrawcallCount() const
	{
		return drawcallCount;
	}

	void Graphics::SetRenderTarget(eRenderTargetType aRenderTargetType, const bool aUseDepth, const bool aWriteToWorldPos) const
	{
		constexpr float colour[] = KAKA_BG_COLOUR;

		switch (aRenderTargetType)
		{
			case eRenderTargetType::Default:
			{
				pContext->OMSetRenderTargets(1u, pDefaultTarget.GetAddressOf(), aUseDepth ? pDepth.Get() : NULL);
				break;
			}
			case eRenderTargetType::WaterReflect:
			{
				ID3D11RenderTargetView* renderTargets[2] = {renderWaterReflect.pTarget.Get(), worldPosition.pTarget.Get()};
				pContext->OMSetRenderTargets(2u, &renderTargets[0], aUseDepth ? pDepth.Get() : NULL);
				break;
			}
			case eRenderTargetType::PostProcessing:
			{
				if (aWriteToWorldPos)
				{
					ID3D11RenderTargetView* renderTargets[2] = {postProcessing.pTarget.Get(), worldPosition.pTarget.Get()};
					pContext->OMSetRenderTargets(2u, &renderTargets[0], aUseDepth ? pDepth.Get() : NULL);
					break;
				}
				else
				{
					pContext->OMSetRenderTargets(1u, postProcessing.pTarget.GetAddressOf(), aUseDepth ? pDepth.Get() : NULL);
					break;
				}
			}
			case eRenderTargetType::ShadowMap:
			{
				pContext->OMSetRenderTargets(0u, nullptr, aUseDepth ? pShadowDepth.Get() : NULL);
				break;
			}
		}
	}

	void Graphics::SetAlphaBlend() const
	{
		pContext->OMSetBlendState(pBlend.Get(), nullptr, 0x0f);
	}

	void Graphics::SetVFXBlend() const
	{
		pContext->OMSetBlendState(pBlendVfx.Get(), nullptr, 0x0f);
	}

	void Graphics::SetAdditiveBlend() const
	{
		pContext->OMSetBlendState(pBlendAdd.Get(), nullptr, 0x0f);
	}

	void Graphics::ResetBlend() const
	{
		pContext->OMSetBlendState(nullptr, nullptr, 0x0f);
	}

	void Graphics::HandleBloomScaling(PostProcessing& aPostProcessor)
	{
		if (useBloom)
		{
			pContext->OMSetRenderTargets(1u, bloomDownscale[0].pTarget.GetAddressOf(), nullptr);
			pContext->PSSetShaderResources(0u, 1u, postProcessing.pResource.GetAddressOf());

			aPostProcessor.SetDownsamplePS();

			bb.uvScale = bloomDivideFactor;
			PixelConstantBuffer<DownSampleBuffer> bloomBuffer{*this, 1u};
			bloomBuffer.Update(*this, bb);
			bloomBuffer.Bind(*this);

			aPostProcessor.Draw(*this);

			for (int i = 1; i < bloomDownscale.size(); ++i)
			{
				pContext->OMSetRenderTargets(1u, bloomDownscale[i].pTarget.GetAddressOf(), nullptr);
				pContext->PSSetShaderResources(0u, 1u, bloomDownscale[i - 1].pResource.GetAddressOf());

				bb.uvScale *= bloomDivideFactor;
				bloomBuffer.Update(*this, bb);
				bloomBuffer.Bind(*this);

				aPostProcessor.Draw(*this);
			}

			SetAlphaBlend();

			aPostProcessor.SetUpsamplePS();

			for (int i = (int)bloomDownscale.size() - 1; i > 0; --i)
			{
				pContext->OMSetRenderTargets(1u, bloomDownscale[i - 1].pTarget.GetAddressOf(), nullptr);
				pContext->PSSetShaderResources(0u, 1u, bloomDownscale[i].pResource.GetAddressOf());

				bb.uvScale /= bloomDivideFactor;
				bloomBuffer.Update(*this, bb);
				bloomBuffer.Bind(*this);

				aPostProcessor.Draw(*this);
			}

			ResetBlend();

			aPostProcessor.SetPostProcessPS();

			pContext->OMSetRenderTargets(1u, pDefaultTarget.GetAddressOf(), nullptr);
			pContext->PSSetShaderResources(0u, 1u, postProcessing.pResource.GetAddressOf());
			pContext->PSSetShaderResources(1u, 1u, bloomDownscale[0].pResource.GetAddressOf());
		}
		else
		{
			pContext->OMSetRenderTargets(1u, pDefaultTarget.GetAddressOf(), nullptr);
			pContext->PSSetShaderResources(0u, 1u, postProcessing.pResource.GetAddressOf());
			ID3D11ShaderResourceView* nullSRVs[1] = {nullptr};
			pContext->PSSetShaderResources(1u, 1, nullSRVs);
		}
	}

	void Graphics::BindWorldPositionTexture()
	{
		pContext->PSSetShaderResources(12u, 1u, worldPosition.pResource.GetAddressOf());
	}

	void Graphics::UnbindWorldPositionTexture()
	{
		ID3D11ShaderResourceView* nullSRVs[1] = {nullptr};
		pContext->PSSetShaderResources(12u, 1u, nullSRVs);
	}

	void Graphics::BindWaterReflectionTexture()
	{
		pContext->PSSetShaderResources(2u, 1u, renderWaterReflect.pResource.GetAddressOf());
	}

	void Graphics::BindPostProcessingTexture()
	{
		pContext->PSSetShaderResources(0u, 1u, postProcessing.pResource.GetAddressOf());
	}

	void Graphics::BindBloomDownscaleTexture(const int aIndex)
	{
		pContext->PSSetShaderResources(0u, 1u, bloomDownscale[aIndex].pResource.GetAddressOf());
	}

	DirectX::XMFLOAT2 Graphics::GetCurrentResolution() const
	{
		return {static_cast<float>(width), static_cast<float>(height)};
	}

	void Graphics::StartShadows(Camera& aCamera, const DirectX::XMFLOAT3 aLightDirection)
	{
		pContext->ClearDepthStencilView(pShadowDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		SetCamera(aCamera);
		aCamera.SetDirection(aLightDirection);

		SetPixelShaderOverride(L"Shaders\\Shadow_PS.cso");

		ID3D11ShaderResourceView* nullSRVs[1] = {nullptr};
		pContext->PSSetShaderResources(14u, 1u, nullSRVs);
	}

	void Graphics::ResetShadows(Camera& aCamera)
	{
		ClearPixelShaderOverride();
		SetCamera(aCamera);
	}

	void Graphics::BindShadows()
	{
		// Show shadow map in new imgui viewport
		ImGui::Begin("Shadow map");
		ImGui::Image(shadowMap.pResource.Get(), ImVec2(1600 / 3, 800 / 3));
		ImGui::End();

		//ImGui::Begin("Depth");
		//ImGui::Image(pDepthShaderResourceView.Get(), ImVec2(1600 / 3, 800 / 3));
		//ImGui::End();

		ImGui::Begin("World Position");
		ImGui::Image(worldPosition.pResource.Get(), ImVec2(1600 / 3, 800 / 3));
		ImGui::End();

		pContext->PSSetSamplers(1u, 1u, pShadowSampler.GetAddressOf());
		pContext->PSSetSamplers(2u, 1u, pShadowCompSampler.GetAddressOf());

		pContext->PSSetShaderResources(14u, 1u, shadowMap.pResource.GetAddressOf());
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

	void Graphics::BindDepth()
	{
		pContext->PSSetShaderResources(13u, 1u, pDepthShaderResourceView.GetAddressOf());
	}

	void Graphics::UnbindDepthSRV()
	{
		ID3D11ShaderResourceView* nullSRVs[1] = {nullptr};
		pContext->PSSetShaderResources(13u, 1, nullSRVs);
	}

	Graphics::FrustumPlanes Graphics::ExtractFrustumPlanes() const
	{
		FrustumPlanes frustum;

		// Extract the rows of the view-projection matrix
		DirectX::XMFLOAT4X4 VP;
		auto viewProjectionMatrix = GetCameraInverseMatrix() * camera->GetProjection();
		DirectX::XMStoreFloat4x4(&VP, viewProjectionMatrix);

		// Extract the frustum planes from the view-projection matrix
		frustum.planes[0] = DirectX::XMFLOAT4(VP._14 + VP._11, VP._24 + VP._21, VP._34 + VP._31, VP._44 + VP._41);
		// Left plane
		frustum.planes[1] = DirectX::XMFLOAT4(VP._14 - VP._11, VP._24 - VP._21, VP._34 - VP._31, VP._44 - VP._41);
		// Right plane
		frustum.planes[2] = DirectX::XMFLOAT4(VP._14 - VP._12, VP._24 - VP._22, VP._34 - VP._32, VP._44 - VP._42);
		// Top plane
		frustum.planes[3] = DirectX::XMFLOAT4(VP._14 + VP._12, VP._24 + VP._22, VP._34 + VP._32, VP._44 + VP._42);
		// Bottom plane
		frustum.planes[4] = DirectX::XMFLOAT4(VP._13, VP._23, VP._33, VP._43); // Near plane
		frustum.planes[5] = DirectX::XMFLOAT4(VP._14 - VP._13, VP._24 - VP._23, VP._34 - VP._33, VP._44 - VP._43);
		// Far plane

		// Normalize the frustum planes
		for (int i = 0; i < 6; ++i)
		{
			float length = std::sqrt(frustum.planes[i].x * frustum.planes[i].x +
				frustum.planes[i].y * frustum.planes[i].y +
				frustum.planes[i].z * frustum.planes[i].z);
			frustum.planes[i] = DirectX::XMFLOAT4(frustum.planes[i].x / length,
			                                      frustum.planes[i].y / length,
			                                      frustum.planes[i].z / length,
			                                      frustum.planes[i].w / length);
		}

		return frustum;
	}

	bool Graphics::IsBoundingBoxInFrustum(const DirectX::XMFLOAT3& aMin, const DirectX::XMFLOAT3& aMax) const
	{
		const FrustumPlanes frustum = ExtractFrustumPlanes();
		for (int i = 0; i < 6; ++i)
		{
			if (frustum.planes[i].x * aMin.x + frustum.planes[i].y * aMin.y + frustum.planes[i].z * aMin.z + frustum.
				planes[i].w > 0.0f)
				continue;
			if (frustum.planes[i].x * aMax.x + frustum.planes[i].y * aMin.y + frustum.planes[i].z * aMin.z + frustum.
				planes[i].w > 0.0f)
				continue;
			if (frustum.planes[i].x * aMin.x + frustum.planes[i].y * aMax.y + frustum.planes[i].z * aMin.z + frustum.
				planes[i].w > 0.0f)
				continue;
			if (frustum.planes[i].x * aMax.x + frustum.planes[i].y * aMax.y + frustum.planes[i].z * aMin.z + frustum.
				planes[i].w > 0.0f)
				continue;
			if (frustum.planes[i].x * aMin.x + frustum.planes[i].y * aMin.y + frustum.planes[i].z * aMax.z + frustum.
				planes[i].w > 0.0f)
				continue;
			if (frustum.planes[i].x * aMax.x + frustum.planes[i].y * aMin.y + frustum.planes[i].z * aMax.z + frustum.
				planes[i].w > 0.0f)
				continue;
			if (frustum.planes[i].x * aMin.x + frustum.planes[i].y * aMax.y + frustum.planes[i].z * aMax.z + frustum.
				planes[i].w > 0.0f)
				continue;
			if (frustum.planes[i].x * aMax.x + frustum.planes[i].y * aMax.y + frustum.planes[i].z * aMax.z + frustum.
				planes[i].w > 0.0f)
				continue;

			// If the bounding box is completely outside any frustum plane, it is not visible
			return false;
		}

		// If the bounding box is not completely outside any frustum plane, it is visible
		return true;
	}
}
