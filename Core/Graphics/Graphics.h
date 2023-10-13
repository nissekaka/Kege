#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <memory>
#include <vector>

#define KAKA_BG_COLOUR {0.1f, 0.2f, 0.3f, 1.0f}

namespace DirectX
{
	struct XMFLOAT3;
}

namespace Kaka
{
	struct Mesh;

	enum class eRenderTargetType
	{
		Default,
		WaterReflect,
		PostProcessing,
	};

	struct RenderTarget
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pResource;
	};

	class Graphics
	{
		friend class Model;
		friend class Bindable;
		friend class DirectionalLight;
		friend class PointLight;
		friend class SpotLight;
		friend class Terrain;
		friend class Skybox;
		friend class ReflectionPlane;
		friend class Game;
		friend class PostProcessing;

	public:
		Graphics(HWND aHWnd, UINT aWidth, UINT aHeight);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();
		void BeginFrame();
		void EndFrame();
		void DrawIndexed(UINT aCount);
		void SetProjection(DirectX::FXMMATRIX& aProjection);
		DirectX::XMMATRIX GetProjection() const;
		void SetCamera(DirectX::FXMMATRIX& aCamera);
		DirectX::XMMATRIX GetCamera() const;
		UINT GetDrawcallCount() const;
		void SetRenderTarget(eRenderTargetType aRenderTargetType) const;
		void SetAlpha() const;
		void ResetAlpha() const;

		void HandleBloomScaling(PostProcessing& aPostProcessor);

		void BindWaterReflectionTexture();
		void BindPostProcessingTexture();
		void BindBloomDownscaleTexture(const int aIndex);
		DirectX::XMFLOAT2 GetCurrentResolution() const;

	public:
		void EnableImGui();
		void DisableImGui();
		bool IsImGuiEnabled() const;
		UINT GetWidth() const;
		UINT GetHeight() const;

	private:
		bool imGuiEnabled = true;
		UINT drawcallCount;

	private:
		struct FrustumPlanes
		{
			DirectX::XMFLOAT4 planes[6];
		};

		FrustumPlanes ExtractFrustumPlanes() const;

	public:
		bool IsBoundingBoxInFrustum(const DirectX::XMFLOAT3& aMin, const DirectX::XMFLOAT3& aMax) const;

	private:
		DirectX::XMMATRIX projection{};
		DirectX::XMMATRIX camera{};
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;

		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlend;

		RenderTarget renderWaterReflect;
		RenderTarget postProcessing;

		std::vector<RenderTarget> bloomDownscale = {};
		int bloomSteps = 5;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pDefaultTarget;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pDefaultShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepth;
		UINT width;
		UINT height;

	private:
		struct DownSampleBuffer
		{
			float bloomBlending = 0.5f;
			float bloomThreshold = 0.25f;
			int uvScale = 2;
			float padding;
		} bb;

		int bloomDivideFactor = 2;
		bool useBloom = false;
	};
}
