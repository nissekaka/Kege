#pragma once
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Windows/Window.h"
#include "Core/Model/FBXLoader.h"
#include <DirectXMath.h>

namespace Kaka
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
	};

	class Game
	{
	public:
		Game();
		// Game loop
		int Go();
	private:
		void Update(const float aDeltaTime);
		void HandleInput(const float aDeltaTime);
	private:
		ImGuiManager imGui;
		Window wnd;
		Timer timer;

		//FBXMesh mesh;
		std::vector<Vertex> vertices = {};
		std::vector<unsigned short> indices = {};
	};
}
