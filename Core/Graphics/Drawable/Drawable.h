#pragma once
#include "Core/Graphics/Graphics.h"

namespace Kaka
{
	class Bindable;

	class Drawable
	{
	public:
		virtual ~Drawable() = default;
		virtual DirectX::XMMATRIX GetTransform() const = 0;
	protected:
		void AddBind(std::unique_ptr<Bindable> aBind);
		void AddIndexBuffer(std::unique_ptr<class IndexBuffer> aIndexBuffer);
		//virtual void Draw(Graphics& aGfx);
	private:
		const IndexBuffer* pIndexBuffer = nullptr;
		std::vector<std::unique_ptr<Bindable>> binds;
	};
}
