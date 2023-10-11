#pragma once
#include "Bindable.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Texture.h"
#include "Topology.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

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

	private:
		VertexBuffer vertexBuffer = {};
		IndexBuffer indexBuffer = {};
		//Texture texture{1u};
		VertexShader vertexShader;
		PixelShader pixelShader;
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
		InputLayout inputLayout;
		Topology topology = {};
	};
}
