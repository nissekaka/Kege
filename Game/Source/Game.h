#pragma once
#include "Core/Windows/Window.h"
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/Camera.h"
#include "Core/Lighting/DirectionalLight.h"
#include "Core/Lighting/PointLight.h"
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
		DirectionalLight directionalLight{1u};
		PointLight pointLight{wnd.Gfx(),2u};
	private:
		bool showDemoWindow = false;
	private:
		float cameraSpeed = 1.0f;
		float cameraSpeedNormal = 1.0f;
		float cameraSpeedBoost = 2.0f;
	private:
		Model spy{wnd.Gfx(),"Assets\\Models\\spy\\spy.fbx"};
		//Model model{wnd.Gfx(), "Assets\\Models\\vamp\\vamp.obj"};
		Model muzen{wnd.Gfx(),"Assets\\Models\\muzen\\MuzenSpeaker.fbx"};
	};
}