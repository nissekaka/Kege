#include "stdafx.h"
#include "ShaderFactory.h"
#include "Core/Graphics/Bindable/PixelShader.h"
#include "Core/Graphics/Bindable/VertexShader.h"

namespace Kaka
{
	PixelShader* ShaderFactory::GetPixelShader(const Graphics& aGfx, const std::wstring& aFileName)
	{
		if (pixelShaders.contains(aFileName))
		{
			return &pixelShaders[aFileName];
		}

		pixelShaders[aFileName] = PixelShader();
		pixelShaders[aFileName].Init(aGfx, aFileName);

		return &pixelShaders[aFileName];
	}

	VertexShader* ShaderFactory::GetVertexShader(const Graphics& aGfx, const std::wstring& aFileName)
	{
		if (vertexShaders.contains(aFileName))
		{
			return &vertexShaders[aFileName];
		}

		vertexShaders[aFileName] = VertexShader();
		vertexShaders[aFileName].Init(aGfx, aFileName);

		return &vertexShaders[aFileName];
	}
}
