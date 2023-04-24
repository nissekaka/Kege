#pragma once
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Windows/Window.h"

namespace Kaka
{
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
	};
}
