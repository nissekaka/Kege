#include "Drawable.h"
#include "Core/Graphics/Bindable/IndexBuffer.h"
#include <cassert>

namespace Kaka
{
	void Drawable::AddBind(std::unique_ptr<Bindable> aBind)
	{
		assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*aBind) != typeid(IndexBuffer));
		binds.push_back(std::move(aBind));
	}

	void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> aIndexBuffer)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = aIndexBuffer.get();
		binds.push_back(std::move(aIndexBuffer));
	}

	void Drawable::Draw(Graphics& aGfx)
	{
		for (const auto& bindable : binds)
		{
			bindable->Bind(aGfx);
		}

		aGfx.DrawIndexed(pIndexBuffer->GetCount());
	}
}
