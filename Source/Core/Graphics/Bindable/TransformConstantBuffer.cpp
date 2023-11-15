#include "stdafx.h"
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
		UpdateBind(aGfx, GetTransforms(aGfx));
	}

	void TransformConstantBuffer::UpdateBind(const Graphics& aGfx, const Transforms& aTransforms)
	{
		pVertexCBuf->Update(aGfx, aTransforms);
		pVertexCBuf->Bind(aGfx);
	}

	TransformConstantBuffer::Transforms TransformConstantBuffer::GetTransforms(const Graphics& aGfx) const
	{
		const DirectX::XMMATRIX objectToWorld = parent.GetTransform();
		DirectX::XMMATRIX objectToClip = objectToWorld * aGfx.GetCamera();
		objectToClip = objectToClip * aGfx.GetProjection();
		return {objectToWorld, objectToClip};

		//		const DirectX::XMMATRIX modelView = parent.GetTransform() * aGfx.GetCamera();
		//return {
		//	DirectX::XMMatrixTranspose(parent.GetTransform()),
		//	DirectX::XMMatrixTranspose(modelView),
		//	DirectX::XMMatrixTranspose(modelView * aGfx.GetProjection())
		//};
	}

	std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transforms>> TransformConstantBuffer::pVertexCBuf;
}
