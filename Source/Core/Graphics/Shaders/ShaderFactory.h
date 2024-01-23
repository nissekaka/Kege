#pragma once
#include <unordered_map>
#include <string>

namespace Kaka
{
	class VertexShader;
	class Graphics;
	class PixelShader;

	class ShaderFactory
	{
	public:
		static PixelShader* GetPixelShader(const Graphics& aGfx, const std::wstring& aFileName);
		static VertexShader* GetVertexShader(const Graphics& aGfx, const std::wstring& aFileName);

	private:
		inline static std::unordered_map<std::wstring, PixelShader> pixelShaders;
		inline static std::unordered_map<std::wstring, VertexShader> vertexShaders;
	};
}
