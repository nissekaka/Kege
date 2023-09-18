#include "Texture.h"
#include <External/include/dxtex/DirectXTex.h>

namespace Kaka
{
	Texture::Texture(const UINT aSlot)
		:
		slot(aSlot) {}

	void Texture::LoadTextureFromModel(const Graphics& aGfx, const std::string& aFilePath)
	{
		DirectX::ScratchImage image;

		// Diffuse
		{
			//DirectX::ScratchImage image;

			// Build the texture path by adding "_c.dds" suffix to the FBX file name
			std::string texturePath = aFilePath;
			const size_t lastDotIndex = texturePath.find_last_of('.');
			texturePath = texturePath.substr(0, lastDotIndex) + "_c.dds";

			DirectX::TexMetadata metadata;

			// Try to load the DDS texture file
			HRESULT hr = DirectX::LoadFromDDSFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
			                                      DirectX::DDS_FLAGS_NONE, &metadata, image);

			if (FAILED(hr))
			{
				// If DDS texture not found, try to load PNG texture with the same name
				texturePath = aFilePath.substr(0, lastDotIndex) + "_c.png";
				hr = DirectX::LoadFromWICFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
				                              DirectX::WIC_FLAGS_NONE, &metadata, image);
			}

			if (SUCCEEDED(hr))
			{
				//if (metadata.format != DXGI_FORMAT_B8G8R8A8_UNORM)
				//{
				//	DirectX::ScratchImage decompressedImage;
				//	// Decompress the texture to a new format (in this case, DXGI_FORMAT_R8G8B8A8_UNORM)
				//	hr = DirectX::Decompress(image.GetImages(), image.GetImageCount(), image.GetMetadata(),
				//	                         DXGI_FORMAT_R8G8B8A8_UNORM, decompressedImage);
				//	image = std::move(decompressedImage);
				//}

				//// Create texture resource
				//D3D11_TEXTURE2D_DESC textureDesc = {};
				//textureDesc.Width = (UINT)metadata.width;
				//textureDesc.Height = (UINT)metadata.height;
				//textureDesc.MipLevels = 1u;
				//textureDesc.ArraySize = 1u;
				//textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				//textureDesc.SampleDesc.Count = 1u;
				//textureDesc.SampleDesc.Quality = 0u;
				//textureDesc.Usage = D3D11_USAGE_DEFAULT;
				//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				//textureDesc.CPUAccessFlags = 0u;
				//textureDesc.MiscFlags = 0u;

				//D3D11_SUBRESOURCE_DATA sd = {};
				//sd.pSysMem = image.GetPixels();
				//sd.SysMemPitch = (UINT)(image.GetPixelsSize() / metadata.height);
				//Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
				//GetDevice(aGfx)->CreateTexture2D(&textureDesc, &sd, &pTexture);

				//// Create the resource view on the texture
				//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				//srvDesc.Format = textureDesc.Format;
				//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				//srvDesc.Texture2D.MostDetailedMip = 0;
				//srvDesc.Texture2D.MipLevels = 1;

				pTextures.emplace_back();
				// Create the shader resource view from the loaded texture
				hr = CreateShaderResourceView(GetDevice(aGfx), image.GetImages(), image.GetImageCount(), metadata,
				                              pTextures.back().GetAddressOf());

				//hr = GetDevice(aGfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextures.back());

				const std::string text =
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++"
					"\nLoaded diffuse texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath +
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++";
				OutputDebugStringA(text.c_str());
			}
			if (FAILED(hr))
			{
				const std::string text =
					"\n-------------------------------------------------"
					"\nFailed to load diffuse texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath +
					"\n-------------------------------------------------";
				OutputDebugStringA(text.c_str());
			}
		}
		// Normal
		{
			//DirectX::ScratchImage image;

			// Build the texture path by adding "_c.dds" suffix to the FBX file name
			std::string texturePath = aFilePath;
			const size_t lastDotIndex = texturePath.find_last_of('.');
			texturePath = texturePath.substr(0, lastDotIndex) + "_n.dds";

			DirectX::TexMetadata metadata;

			// Try to load the DDS texture file
			HRESULT hr = DirectX::LoadFromDDSFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
			                                      DirectX::DDS_FLAGS_NONE, &metadata, image);

			if (FAILED(hr))
			{
				// If DDS texture not found, try to load PNG texture with the same name
				texturePath = aFilePath.substr(0, lastDotIndex) + "_n.png";
				hr = DirectX::LoadFromWICFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
				                              DirectX::WIC_FLAGS_NONE, &metadata, image);
			}

			if (SUCCEEDED(hr))
			{
				//// Create texture resource
				//D3D11_TEXTURE2D_DESC textureDesc = {};
				//textureDesc.Width = (UINT)metadata.width;
				//textureDesc.Height = (UINT)metadata.height;
				//textureDesc.MipLevels = 1;
				//textureDesc.ArraySize = 1;
				//textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				//textureDesc.SampleDesc.Count = 1;
				//textureDesc.SampleDesc.Quality = 0;
				//textureDesc.Usage = D3D11_USAGE_DEFAULT;
				//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				//textureDesc.CPUAccessFlags = 0;
				//textureDesc.MiscFlags = 0;

				//D3D11_SUBRESOURCE_DATA sd = {};
				//sd.pSysMem = image.GetPixels();
				//sd.SysMemPitch = (UINT)(image.GetPixelsSize() / metadata.height);
				//Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
				//GetDevice(aGfx)->CreateTexture2D(&textureDesc, &sd, &pTexture);

				//// Create the resource view on the texture
				//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				//srvDesc.Format = textureDesc.Format;
				//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				//srvDesc.Texture2D.MostDetailedMip = 0;
				//srvDesc.Texture2D.MipLevels = 1;

				pTextures.emplace_back();
				// Create the shader resource view from the loaded texture
				hr = CreateShaderResourceView(GetDevice(aGfx), image.GetImages(), image.GetImageCount(), metadata,
				                              pTextures.back().GetAddressOf());

				//hr = GetDevice(aGfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextures.back());

				const std::string text =
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++"
					"\nLoaded normal texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath +
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++";
				OutputDebugStringA(text.c_str());
				hasNormalMap = TRUE;
			}
			if (FAILED(hr))
			{
				const std::string text =
					"\n-------------------------------------------------"
					"\nFailed to load normal texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath +
					"\n-------------------------------------------------";
				OutputDebugStringA(text.c_str());
			}
		}
		// Material
		{
			//DirectX::ScratchImage image;

			// Build the texture path by adding "_c.dds" suffix to the FBX file name
			std::string texturePath = aFilePath;
			const size_t lastDotIndex = texturePath.find_last_of('.');
			texturePath = texturePath.substr(0, lastDotIndex) + "_m.dds";

			DirectX::TexMetadata metadata;

			// Try to load the DDS texture file
			HRESULT hr = DirectX::LoadFromDDSFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
			                                      DirectX::DDS_FLAGS_NONE, &metadata, image);

			if (FAILED(hr))
			{
				// If DDS texture not found, try to load PNG texture with the same name
				texturePath = aFilePath.substr(0, lastDotIndex) + "_m.png";
				hr = DirectX::LoadFromWICFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
				                              DirectX::WIC_FLAGS_NONE, &metadata, image);
			}

			if (SUCCEEDED(hr))
			{
				//// Create texture resource
				//D3D11_TEXTURE2D_DESC textureDesc = {};
				//textureDesc.Width = (UINT)metadata.width;
				//textureDesc.Height = (UINT)metadata.height;
				//textureDesc.MipLevels = 1;
				//textureDesc.ArraySize = 1;
				//textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				//textureDesc.SampleDesc.Count = 1;
				//textureDesc.SampleDesc.Quality = 0;
				//textureDesc.Usage = D3D11_USAGE_DEFAULT;
				//textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				//textureDesc.CPUAccessFlags = 0;
				//textureDesc.MiscFlags = 0;

				//D3D11_SUBRESOURCE_DATA sd = {};
				//sd.pSysMem = image.GetPixels();
				//sd.SysMemPitch = (UINT)(image.GetPixelsSize() / metadata.height);
				//Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
				//GetDevice(aGfx)->CreateTexture2D(&textureDesc, &sd, &pTexture);

				//// Create the resource view on the texture
				//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				//srvDesc.Format = textureDesc.Format;
				//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				//srvDesc.Texture2D.MostDetailedMip = 0;
				//srvDesc.Texture2D.MipLevels = 1;

				pTextures.emplace_back();
				// Create the shader resource view from the loaded texture
				hr = CreateShaderResourceView(GetDevice(aGfx), image.GetImages(), image.GetImageCount(), metadata,
				                              pTextures.back().GetAddressOf());

				//hr = GetDevice(aGfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextures.back());

				const std::string text =
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++"
					"\nLoaded material texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath +
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++";
				OutputDebugStringA(text.c_str());
				hasMaterial = TRUE;
			}
			if (FAILED(hr))
			{
				const std::string text =
					"\n-------------------------------------------------"
					"\nFailed to load material texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath +
					"\n-------------------------------------------------";
				OutputDebugStringA(text.c_str());
			}
		}
	}

	void Texture::LoadTextureFromPath(const Graphics& aGfx, const std::string& aFilePath)
	{
		DirectX::ScratchImage image;
		DirectX::TexMetadata metadata;

		// Try to load the DDS texture file
		HRESULT hr = DirectX::LoadFromDDSFile(std::wstring(aFilePath.begin(), aFilePath.end()).c_str(),
		                                      DirectX::DDS_FLAGS_NONE, &metadata, image);

		if (FAILED(hr))
		{
			// If DDS texture not found, try to load PNG texture with the same name
			hr = DirectX::LoadFromWICFile(std::wstring(aFilePath.begin(), aFilePath.end()).c_str(),
			                              DirectX::WIC_FLAGS_NONE, &metadata, image);
		}

		if (SUCCEEDED(hr))
		{
			// Generate mipmaps for the loaded texture
			DirectX::ScratchImage mipmappedImage;
			hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), metadata,
			                              DirectX::TEX_FILTER_DEFAULT, 0, mipmappedImage);

			if (SUCCEEDED(hr))
			{
				pTextures.emplace_back();
				// Create the shader resource view from the mipmapped texture
				hr = CreateShaderResourceView(GetDevice(aGfx), mipmappedImage.GetImages(),
				                              mipmappedImage.GetImageCount(), mipmappedImage.GetMetadata(),
				                              pTextures.back().GetAddressOf());
			}

			if (SUCCEEDED(hr))
			{
				const std::string text =
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++"
					"\nLoaded texture!"
					"\nTexture: " + aFilePath +
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++";
				OutputDebugStringA(text.c_str());
			}
		}

		if (FAILED(hr))
		{
			const std::string text =
				"\n-------------------------------------------------"
				"\nFailed to load texture!"
				"\nTexture: " + aFilePath +
				"\n-------------------------------------------------";
			OutputDebugStringA(text.c_str());
		}
	}

	void Texture::Bind(const Graphics& aGfx)
	{
		//for (UINT i = 0; i < (UINT)pTextures.size(); ++i)
		//{
		//	GetContext(aGfx)->PSSetShaderResources(slot + i, 1u, pTextures[i].GetAddressOf());
		//}
		GetContext(aGfx)->PSSetShaderResources(slot, static_cast<UINT>(pTextures.size()),
		                                       pTextures.data()->GetAddressOf());
	}

	BOOL Texture::HasNormalMap() const
	{
		return hasNormalMap;
	}

	BOOL Texture::HasMaterial() const
	{
		return hasMaterial;
	}
}
