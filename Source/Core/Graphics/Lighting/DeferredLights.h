#pragma once
#include "Lighting.h"
#include "Graphics/Bindable/IndexBuffer.h"
#include "Graphics/Bindable/InputLayout.h"
#include "Graphics/Bindable/Topology.h"
#include "Graphics/Bindable/VertexBuffer.h"

namespace Kaka
{
	class PixelShader;
	class VertexShader;

	class DeferredLights
	{
	public:
		DeferredLights() = default;
		~DeferredLights() = default;

		void CreateSphere(Graphics& aGfx);
		void CreateQuad(Graphics& aGfx);
		void Init(Graphics& aGfx);
		void Draw(Graphics& aGfx);
		void ShowControlWindow();
		void SetShadowCamera(const DirectX::XMMATRIX& aCamera);
		PointLightData& AddPointLight();
		SpotLightData& AddSpotLight();

		DirectionalLightData& GetDirectionalLightData() { return directionalLightData; }

	private:
		struct ConstantBuffer
		{
			float positionAndRange[4];
			BOOL isDirectional;
			float padding[3];
		};

		struct PVertex
		{
			DirectX::XMFLOAT3 pos;
		};

		VertexShader* lightVS = nullptr;

		DirectionalLightData directionalLightData;
		std::vector<PointLightData> pointLightData;
		std::vector<SpotLightData> spotLightData;

		PixelShader* directionalLightPS = nullptr;
		PixelShader* pointLightPS = nullptr;
		PixelShader* spotLightPS = nullptr;

		// Quad
		VertexBuffer quadVertexBuffer = {};
		IndexBuffer quadIndexBuffer = {};
		// Sphere
		VertexBuffer sphereVertexBuffer = {};
		IndexBuffer sphereIndexBuffer = {};

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
		InputLayout inputLayout;
		Topology topology = {};
	};
}
