#include "TransformConstantBuffer.h"

namespace Kaka
{
	TransformConstantBuffer::TransformConstantBuffer(const Graphics& aGfx, const Drawable& aParent, const UINT aSlot)
		:
		parent(aParent)
	{
		if (!pVertexCBuf)
		{
			pVertexCBuf = std::make_unique<VertexConstantBuffer<Transforms>>(aGfx, aSlot);
		}
	}

	void TransformConstantBuffer::Bind(const Graphics& aGfx)
	{
		//const DirectX::XMMATRIX modelView = parent.GetTransform() * aGfx.GetCamera();
		const Transforms transforms =
		{
			XMMatrixTranspose(parent.GetTransform()),
			XMMatrixTranspose(aGfx.GetCamera()),
			XMMatrixTranspose(aGfx.GetProjection())
		};
		pVertexCBuf->Update(aGfx, transforms);
		pVertexCBuf->Bind(aGfx);
	}

	std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transforms>> TransformConstantBuffer::pVertexCBuf;
}
