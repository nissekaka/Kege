#pragma once
#include <unordered_map>
#include <string>

namespace Kaka
{
	class Graphics;
	class VertexShader;
	class PixelShader;
	class ComputeShader;

	class ShaderFactory
	{
	public:
		static PixelShader* GetPixelShader(const Graphics& aGfx, const std::wstring& aFileName);
		static VertexShader* GetVertexShader(const Graphics& aGfx, const std::wstring& aFileName);
		static ComputeShader* GetComputeShader(const Graphics& aGfx, const std::wstring& aFileName);

	private:
		inline static std::unordered_map<std::wstring, PixelShader> pixelShaders;
		inline static std::unordered_map<std::wstring, VertexShader> vertexShaders;
		inline static std::unordered_map<std::wstring, ComputeShader> computeShaders;
	};
}
