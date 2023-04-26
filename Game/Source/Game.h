#pragma once
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/Camera.h"
#include "Core/Windows/Window.h"
#include "Core/Model/Model.h"

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
		Camera camera;
	private:
		bool showDemoWindow = false;
	private:
		float cameraSpeed = 1.0f;
		float cameraSpeedNormal = 1.0f;
		float cameraSpeedBoost = 2.0f;
	private:
		Model model{"Assets\\Models\\spy\\spy.fbx"};
		Model model2{"Assets\\Models\\muzen\\MuzenSpeaker.fbx"};
	};
}