#pragma once
#include "Core/Windows/Window.h"
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/Camera.h"
#include <Core/Utility/KakaMath.h>
#include "Core/Lighting/DirectionalLight.h"
#include "Core/Lighting/PointLight.h"
#include "Core/Model/Model.h"
#include <array>

#include "Game/Source/Terrain.h"

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
		void ShowStatsWindow();
	private:
		ImGuiManager imGui;
		Window wnd;
		Timer timer;
		Camera camera;

		DirectionalLight directionalLight{wnd.Gfx(),1u};
		std::vector<PointLight> pointLights;
		float angle[4] = {0.0f,PI / 2,PI,PI * 1.5f};

	private:
		bool showImGui = true;
		bool showStatsWindow = true;
		bool drawLightDebug = false;
	private:
		float cameraSpeed = 1.0f;
		float cameraSpeedNormal = 1.0f;
		float cameraSpeedBoost = 2.0f;
	private:
		Model spy{};
		Model muzen{};
		Model vamp{};
		Model cube{};
		Terrain terrain{};
	};
}