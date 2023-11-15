#pragma once
#include "Core/Graphics/Bindable/Bindable.h"
#include "Core/Graphics/Bindable/IndexBuffer.h"
#include "Core/Graphics/Bindable/InputLayout.h"
#include "Core/Graphics/Bindable/PixelShader.h"
#include "Core/Graphics/Bindable/Texture.h"
#include "Core/Graphics/Bindable/Topology.h"
#include "Core/Graphics/Bindable/VertexBuffer.h"
#include "Core/Graphics/Bindable/VertexShader.h"

namespace Kaka
{
	class Texture;
	class PixelShader;
	class VertexShader;

	class PostProcessing
	{
	public:
		PostProcessing();

		void Init(const Graphics& aGfx);
		void Draw(Graphics& aGfx);
		void SetDownsamplePS();
		void SetUpsamplePS();
		void SetPostProcessPS();

	private:
		VertexBuffer vertexBuffer = {};
		IndexBuffer indexBuffer = {};
		//Texture texture{1u};
		VertexShader postProcessVS;
		PixelShader postProcessPS;
		PixelShader downsamplePS;
		PixelShader upsamplePS;

		PixelShader* currentPS = nullptr;

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
		InputLayout inputLayout;
		Topology topology = {};
	};
}
