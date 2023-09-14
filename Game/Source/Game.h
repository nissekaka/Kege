#pragma once
#include "Core/Windows/Window.h"
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/Camera.h"
#include <Core/Utility/KakaMath.h>
#include "Core/Graphics/Lighting/DirectionalLight.h"
#include "Core/Graphics/Lighting/PointLight.h"
#include "Core/Graphics/Lighting/SpotLight.h"
#include "Core/Graphics/Drawable/Model.h"
#include "Core/Graphics/Drawable/Terrain.h"
#include "Core/Graphics/Drawable/Skybox.h"
#include "Core/Graphics/Drawable/ReflectionPlane.h"

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

		DirectionalLight directionalLight{wnd.Gfx(), 1u};
		std::vector<PointLight> pointLights = {};
		std::vector<SpotLight> spotLights = {};
		//float pointLightAngles[] = PI;
		//float pointLightAngles[4] = {0.0f,PI / 2,PI,PI * 1.5f};

	private:
		bool showImGui = true;
		bool showStatsWindow = true;
		bool drawLightDebug = false;

	private:
		float cameraSpeed = 1.0f;
		float cameraSpeedDefault = 1.0f;
		float cameraSpeedBoost = 2.0f;

	private:
		Model spy{};
		Model muzen{};
		Model vamp{};
		Model cube{};
		Model cubeTwoBones{};
		Terrain terrain{};
		Skybox skybox{};
		ReflectionPlane reflectionPlane{};
		float skyboxSpeed = 0.005f;
		DirectX::XMFLOAT3 skyboxAngle = {};
		std::vector<float> pointLightTravelRadiuses = {};
		std::vector<float> pointLightTravelSpeeds = {};
		std::vector<float> pointLightTravelAngles = {};

		std::vector<float> spotLightTravelRadiuses = {};
		std::vector<float> spotLightTravelSpeeds = {};
		std::vector<float> spotLightTravelAngles = {};

		struct ReflectionWaveBuffer
		{
			DirectX::XMFLOAT2 k0;
			DirectX::XMFLOAT2 k1;
			float A;
			float padding[3];
		};

		ReflectionWaveBuffer reflectionPSBuffer = {};

		struct ReflectionHeightBuffer
		{
			float height;
			float padding[3];
		};

		ReflectionHeightBuffer reflectionVSBuffer = {};
		ReflectionHeightBuffer reflectionHeightPSBuffer = {};

		struct CommonBuffer
		{
			DirectX::XMFLOAT4 cameraPosition;
			DirectX::XMFLOAT2 resolution;
			float currentTime;
			float padding;
		};

		CommonBuffer commonBuffer = {};
	};
}
