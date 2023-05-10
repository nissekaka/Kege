#pragma once
#include "Core/Windows/Window.h"
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/Camera.h"
#include "Core/Lighting/DirectionalLight.h"
#include "Core/Lighting/PointLight.h"
#include "Core/Model/Model.h"
#include <array>

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
		DirectionalLight directionalLight{wnd.Gfx(), 1u};
		//std::array<PointLight, 4> pointLights = {
		//	PointLight{wnd.Gfx(), 2u},
		//	PointLight{wnd.Gfx(), 2u},
		//	PointLight{wnd.Gfx(), 2u},
		//	PointLight{wnd.Gfx(), 2u}
		//};
		//std::array<Model, 4> pointLightModels = {
		//	Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
		//	Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
		//	Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
		//	Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid}
		//};


		std::array<PointLight, 100> pointLights = {
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u},
			PointLight{wnd.Gfx(), 2u}
		};
		std::array<Model, 100> pointLightModels = {
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid},
			Model{wnd.Gfx(), "Assets\\Models\\pointlight\\pointlight.fbx", Model::eShaderType::Solid}
		};
	private:
		bool showDemoWindow = false;
	private:
		float cameraSpeed = 1.0f;
		float cameraSpeedNormal = 1.0f;
		float cameraSpeedBoost = 2.0f;
	private:
		//Model spy{wnd.Gfx(), "Assets\\Models\\spy\\spy.fbx", Model::eShaderType::Light};
		//Model model{wnd.Gfx(), "Assets\\Models\\vamp\\vamp.obj"};
		Model muzen{wnd.Gfx(), "Assets\\Models\\muzen\\MuzenSpeaker.fbx", Model::eShaderType::Phong};
	};
}
