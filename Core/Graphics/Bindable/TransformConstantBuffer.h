#pragma once
#include "ConstantBuffers.h"
#include "Core/Model/Drawable.h"
#include <DirectXMath.h>

namespace Kaka
{
	class TransformConstantBuffer : public Bindable
	{
	private:
		struct Transforms
		{
			DirectX::XMMATRIX transform;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;
		};

	public:
		TransformConstantBuffer(const Graphics& aGfx, const Drawable& aParent, const UINT aSlot = 0u);
		void Bind(const Graphics& aGfx) override;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVertexCBuf;
		const Drawable& parent;
	};
}
