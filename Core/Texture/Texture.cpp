#include "Texture.h"
#include "Core/Utility/KakaUtility.h"

namespace Kaka
{
	Texture::Texture() {}

	Texture Texture::LoadTexture(const std::string& aName)
	{
		DirectX::ScratchImage scratch;
		HRESULT hr = DirectX::LoadFromDDSFile(ToWide(aName).c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scratch);

		assert(FAILED(hr) && "Failed to load DDS file");

		if (scratch.GetImage(0, 0, 0)->format != FORMAT)
		{
			DirectX::ScratchImage converted;
			hr = DirectX::Convert(
				*scratch.GetImage(0, 0, 0),
				FORMAT,
				DirectX::TEX_FILTER_DEFAULT,
				DirectX::TEX_THRESHOLD_DEFAULT,
				converted
			);

			assert(FAILED(hr) && "Failed to convert image");

			return Texture(std::move(converted));
		}

		return Texture(std::move(scratch));
	}

	Texture::Texture(DirectX::ScratchImage aScratch)
		:
		scratch(std::move(aScratch)) {}
}