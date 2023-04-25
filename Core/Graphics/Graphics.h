#pragma once
#include <wrl.h>
#include <d3d11.h>

#define KAKA_BG_COLOUR {0.1f, 0.2f, 0.3f, 1.0f}

namespace DirectX
{
	struct XMFLOAT3;
}

namespace Kaka
{
	struct Mesh;

	class Graphics
	{
		friend class Model;
	public:
		Graphics(HWND aHWnd, UINT aWidth, UINT aHeight);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();
		void BeginFrame();
		void EndFrame();
		void EnableImGui();
		void DisableImGui();
		bool IsImGuiEnabled() const;
		UINT GetWidth() const;
		UINT GetHeight() const;
	private:
		bool imGuiEnabled = true;
	public:
		void DrawTestTriangle2D();
		void DrawTestCube3D(const float aAngle, const DirectX::XMFLOAT3 aPos);
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepth;
		UINT width;
		UINT height;
	};
}