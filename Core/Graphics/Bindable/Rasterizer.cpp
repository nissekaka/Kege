#include "Rasterizer.h"

namespace Kaka
{
	Rasterizer::Rasterizer(const Graphics& aGfx, const bool aTwoSided)
		:
		twoSided(aTwoSided)
	{
		Init(aGfx, aTwoSided);
	}

	void Rasterizer::Init(const Graphics& aGfx, const bool aTwoSided)
	{
		twoSided = aTwoSided;
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

		GetDevice(aGfx)->CreateRasterizerState(&rasterDesc, &pRasterizer);
	}

	void Rasterizer::Bind(const Graphics& aGfx)
	{
		GetContext(aGfx)->RSSetState(pRasterizer.Get());
	}
}
