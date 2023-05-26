#pragma once

#include "Bindable.h"
#include <string>

namespace Kaka
{
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer() = default;
		Rasterizer(const Graphics& aGfx, bool aTwoSided);
		void Init(const Graphics& aGfx, bool aTwoSided);
		void Bind(const Graphics& aGfx) override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		bool twoSided;
	};
}
