#include "CubeTexture.h"
#include <External/include/dxtex/DirectXTex.h>

namespace Kaka
{
	CubeTexture::CubeTexture(const UINT aSlot)
		:
		slot(aSlot) {}

	void CubeTexture::LoadTextures(const Graphics& aGfx, const std::string& aFolderPath)
	{
		DirectX::ScratchImage image;
		DirectX::TexMetadata metadata;

		for (int i = 0; i < 6; ++i)
		{
			std::string path = aFolderPath + std::to_string(i) + ".jpg";
			// Try to load the DDS texture file
			HRESULT hr = DirectX::LoadFromDDSFile(std::wstring(path.begin(), path.end()).c_str(),
			                                      DirectX::DDS_FLAGS_NONE, &metadata, image);

			if (FAILED(hr))
			{
				// If DDS texture not found, try to load PNG texture with the same name
				hr = DirectX::LoadFromWICFile(std::wstring(path.begin(), path.end()).c_str(),
				                              DirectX::WIC_FLAGS_NONE, &metadata, image);
			}

			if (SUCCEEDED(hr))
			{
				pTextures.emplace_back();
				// Create the shader resource view from the mipmapped texture
				hr = CreateShaderResourceView(GetDevice(aGfx), image.GetImages(),
				                              image.GetImageCount(), image.GetMetadata(),
				                              pTextures.back().GetAddressOf());
			}

			if (SUCCEEDED(hr))
			{
				const std::string text =
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++"
					"\nLoaded cube texture!"
					"\nCube texture: " + path +
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++";
				OutputDebugStringA(text.c_str());
			}

			if (FAILED(hr))
			{
				const std::string text =
					"\n-------------------------------------------------"
					"\nFailed to load cube texture!"
					"\nCube texture: " + path +
					"\n-------------------------------------------------";
				OutputDebugStringA(text.c_str());
			}
		}
	}

	void CubeTexture::Bind(const Graphics& aGfx)
	{
		UNREFERENCED_PARAMETER(aGfx);
	}
}