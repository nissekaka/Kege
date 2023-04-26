#pragma once
#include <External/include/dxtex/DirectXTex.h>
#include <string>
#include <d3d11.h>
#include <stdexcept>
#include <wrl/client.h>

namespace Kaka
{
	class Texture
	{
	public:
		Texture() = default;
		~Texture() = default;
		void LoadTexture(ID3D11Device* aPDevice, const std::string& aFilePath);
		void Bind(ID3D11DeviceContext* aPContext, unsigned int aSlot = 0u) const;
		//ID3D11ShaderResourceView* GetTexture();

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture;
	};
}