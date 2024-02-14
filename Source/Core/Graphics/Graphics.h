#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <memory>
#include <vector>

#include "GBuffer.h"
#include "Shaders/ShaderFactory.h"


#define KAKA_BG_COLOUR {0.1f, 0.2f, 0.3f, 1.0f}

namespace DirectX
{
	struct XMFLOAT3;
}

namespace Kaka
{
	class Texture;
	class VertexShader;
	class PixelShader;
	class Camera;
	struct Mesh;

	enum class eRenderTargetType
	{
		None,
		Default,
		WaterReflect,
		PostProcessing,
		ShadowMap,
	};

	enum class eBlendStates
	{
		Disabled,
		Alpha,
		Additive,
		TransparencyBlend,
		VFX,
		Count,
	};

	enum class eDepthStencilStates
	{
		Normal,
		ReadOnlyGreater,
		ReadOnlyLessEqual,
		ReadOnlyEmpty,
		Count,
	};

	enum class eRasterizerStates
	{
		BackfaceCulling,
		FrontfaceCulling,
		NoCulling,
		Count,
	};

	struct RenderTarget
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pResource;
	};

	class Graphics
	{
		friend class GBuffer;
		friend class DeferredLights;
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
		friend class Sprite;

	public:
		Graphics(HWND aHWnd, UINT aWidth, UINT aHeight);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();
		void BeginFrame();
		void EndFrame();
		void DrawIndexed(UINT aCount);
		void DrawIndexedInstanced(UINT aCount, UINT aInstanceCount);
		//void SetProjection(DirectX::FXMMATRIX& aProjection);
		DirectX::XMMATRIX GetProjection() const;
		void SetCamera(Camera& aCamera);
		DirectX::XMMATRIX GetCameraInverseView() const;
		UINT GetDrawcallCount() const;
		void SetRenderTarget(eRenderTargetType aRenderTargetType, const bool aUseDepth = true) const;
		void SetRenderTarget(eRenderTargetType aRenderTargetType, ID3D11DepthStencilView* aDepth) const;
		void SetAlphaBlend() const;
		void SetVFXBlend() const;
		void SetAdditiveBlend() const;
		void ResetBlend() const;

		void HandleBloomScaling(PostProcessing& aPostProcessor);

		void SetBlendState(eBlendStates aBlendState);
		void SetDepthStencilState(eDepthStencilStates aDepthStencilState);
		void SetRasterizerState(eRasterizerStates aRasterizerState);

		bool CreateBlendStates();
		bool CreateDepthStencilStates();
		bool CreateRasterizerStates();

		//void BindWorldPositionTexture();
		//void UnbindWorldPositionTexture();
		void BindWaterReflectionTexture();
		void BindPostProcessingTexture();
		void BindBloomDownscaleTexture(const int aIndex);
		DirectX::XMFLOAT2 GetCurrentResolution() const;

		void StartShadows(Camera& aCamera, DirectX::XMFLOAT3 aLightDirection);
		void ResetShadows(Camera& aCamera);
		void BindShadows();
		void UnbindShadows();

		void SetPixelShaderOverride(const std::wstring& aFileName) { pixelShaderOverride = ShaderFactory::GetPixelShader(*this, aFileName); }
		void SetVertexShaderOverride(const std::wstring& aFileName) { vertexShaderOverride = ShaderFactory::GetVertexShader(*this, aFileName); }
		PixelShader* GetPixelShaderOverride() const { return pixelShaderOverride; }
		VertexShader* GetVertexShaderOverride() const { return vertexShaderOverride; }
		bool HasPixelShaderOverride() const { return pixelShaderOverride != nullptr; }
		bool HasVertexShaderOverride() const { return vertexShaderOverride != nullptr; }
		void ClearPixelShaderOverride() { pixelShaderOverride = nullptr; }
		void ClearVertexShaderOverride() { vertexShaderOverride = nullptr; }

	public:
		void EnableImGui();
		void DisableImGui();
		bool IsImGuiEnabled() const;
		UINT GetWidth() const;
		UINT GetHeight() const;
		void BindDepth();
		void UnbindDepthSRV();

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
		Camera* camera = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;

		//Microsoft::WRL::ComPtr<ID3D11BlendState> pBlend;
		//Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendVfx;
		//Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendAdd;

		RenderTarget renderWaterReflect;
		RenderTarget postProcessing;

		std::vector<RenderTarget> bloomDownscale = {};
		int bloomSteps = 5;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pDefaultTarget;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pDepthShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepth;

		//RenderTarget worldPosition;

		RenderTarget shadowMap;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pShadowDepth;

		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendStates[(int)eBlendStates::Count];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilStates[(int)eDepthStencilStates::Count];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerStates[(int)eRasterizerStates::Count];

		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pShadowSampler;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pShadowCompSampler;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pVFXSampler;

		UINT width;
		UINT height;

		PixelShader* pixelShaderOverride = nullptr;
		VertexShader* vertexShaderOverride = nullptr;

		GBuffer gBuffer;

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
