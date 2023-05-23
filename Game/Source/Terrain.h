#pragma once
#include "Core/Graphics/Graphics.h"
#include "Core/Graphics/Bindable/BindableCommon.h"
#include "Core/Model/Drawable.h"

namespace Kaka
{
	struct Vertex;

	class Terrain : public Drawable
	{
	public:
		void Init(const Graphics& aGfx, int aSize);
		void Draw(const Graphics& aGfx);
		DirectX::XMMATRIX GetTransform() const override;

	private:
		std::vector<Vertex> tVertices;
		std::vector<unsigned short> tIndices;
		Texture texture;
	};
}