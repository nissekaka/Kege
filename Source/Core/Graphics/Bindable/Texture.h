#pragma once
#include "Bindable.h"
#include <string>
#include <d3d11.h>
#include <stdexcept>
#include <wrl/client.h>


namespace Kaka
{
	class Texture : public Bindable
	{
	public:
		Texture(const UINT aSlot = 0u);
		~Texture() override = default;
		void LoadTextureFromModel(const Graphics& aGfx, const std::string& aFilePath);
		void LoadTextureFromPath(const Graphics& aGfx, const std::string& aFilePath);
		void Bind(const Graphics& aGfx) override;
		BOOL HasNormalMap() const;
		BOOL HasMaterial() const;
		void SetSlot(const UINT aSlot) { slot = aSlot; }

	private:
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> pTextures;
		UINT slot;
		BOOL hasNormalMap = FALSE;
		BOOL hasMaterial = FALSE;
	};

	struct Material
	{
		std::string name;
		Texture* textures[2u];
	};
}
