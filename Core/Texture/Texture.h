#pragma once
#include <External/include/dxtex/DirectXTex.h>
#include <string>

namespace Kaka
{
	class Texture
	{
	public:
		Texture();
		~Texture() = default;
		static Texture LoadTexture(const std::string& aName);
	private:
		Texture(DirectX::ScratchImage aScratch);
	private:
		DirectX::ScratchImage scratch;
		static constexpr DXGI_FORMAT FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;
	};
}