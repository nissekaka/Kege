#pragma once
#include "Bindable.h"

namespace Kaka
{
	class Sampler : public Bindable
	{
	public:
		Sampler(const Graphics& aGfx, const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx) override;
	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
	};
}
