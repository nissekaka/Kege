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

		void Init(Graphics& aGfx);
		void Draw(Graphics& aGfx);

		void ShowControlWindow();

	private:
		VertexShader* lightVS = nullptr;

		PixelShader* directionalLightPS = nullptr;
		DirectionalLightData directionalLightData;

		// Quad
		VertexBuffer vertexBuffer = {};
		IndexBuffer indexBuffer = {};
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
		InputLayout inputLayout;
		Topology topology = {};
	};
}
