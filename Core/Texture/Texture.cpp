#include "Texture.h"
#include "Core/Utility/KakaUtility.h"

namespace Kaka
{
	void Texture::LoadTexture(ID3D11Device* aPDevice, const std::string& aFilePath)
	{
		DirectX::ScratchImage image;

		// Diffuse
		{
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
				// Create the shader resource view from the loaded texture
				hr = CreateShaderResourceView(aPDevice, image.GetImages(), image.GetImageCount(), metadata,
				                              pTexture.GetAddressOf());
				const std::string text =
					"\n+++++++++++++++++++++++++++++++++++++++++++++++++"
					"\nLoaded diffuse texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath;
				OutputDebugStringA(text.c_str());
			}
			if (FAILED(hr))
			{
				//throw std::runtime_error("Failed to load texture: " + texturePath);
				const std::string text =
					"\n-------------------------------------------------"
					"\nFailed to load diffuse texture!"
					"\nModel: " + aFilePath +
					"\nTexture: " + texturePath;
				OutputDebugStringA(text.c_str());
			}
		}
		// Normal
		//{
		//	// Build the texture path by adding "_c.dds" suffix to the FBX file name
		//	std::string texturePath = aFilePath;
		//	const size_t lastDotIndex = texturePath.find_last_of('.');
		//	texturePath = texturePath.substr(0, lastDotIndex) + "_n.dds";

		//	DirectX::TexMetadata metadata;

		//	// Try to load the DDS texture file
		//	HRESULT hr = DirectX::LoadFromDDSFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
		//	                                      DirectX::DDS_FLAGS_NONE, &metadata, image);

		//	if (FAILED(hr))
		//	{
		//		// If DDS texture not found, try to load PNG texture with the same name
		//		texturePath = aFilePath.substr(0, lastDotIndex) + "_n.png";
		//		hr = DirectX::LoadFromWICFile(std::wstring(texturePath.begin(), texturePath.end()).c_str(),
		//		                              DirectX::WIC_FLAGS_NONE, &metadata, image);
		//	}

		//	if (SUCCEEDED(hr))
		//	{
		//		// Create the shader resource view from the loaded texture
		//		hr = CreateShaderResourceView(aPDevice, image.GetImages(), image.GetImageCount(), metadata,
		//		                              pTexture.GetAddressOf());
		//		const std::string text =
		//			"\n+++++++++++++++++++++++++++++++++++++++++++++++++"
		//			"\nLoaded normal texture!"
		//			"\nModel: " + aFilePath +
		//			"\nTexture: " + texturePath;
		//		OutputDebugStringA(text.c_str());
		//	}
		//	if (FAILED(hr))
		//	{
		//		//throw std::runtime_error("Failed to load texture: " + texturePath);
		//		const std::string text =
		//			"\n-------------------------------------------------"
		//			"\nFailed to load normal texture!"
		//			"\nModel: " + aFilePath +
		//			"\nTexture: " + texturePath;
		//		OutputDebugStringA(text.c_str());
		//	}
		//}
	}

	void Texture::Bind(ID3D11DeviceContext* aPContext, const unsigned int aSlot) const
	{
		aPContext->PSSetShaderResources(aSlot, 1u, pTexture.GetAddressOf());
	}

	//ID3D11ShaderResourceView* Texture::GetTexture()
	//{
	//	return pTexture.Get();
	//}
}
