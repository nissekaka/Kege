#pragma once
#include "Bindable.h"

namespace Kaka
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout(const Graphics& aGfx,
		            const std::vector<D3D11_INPUT_ELEMENT_DESC>& aLayout,
		            ID3DBlob* aPVertexShaderBytecode);
		void Bind(const Graphics& aGfx) override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
