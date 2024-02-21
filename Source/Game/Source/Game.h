#pragma once
#include "Core/Windows/Window.h"
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/Camera.h"
#include <Core/Utility/KakaMath.h>

#include "Core/Graphics/PostProcessing/PostProcessing.h"
#include "Core/Graphics/Lighting/DirectionalLight.h"
#include "Core/Graphics/Lighting/PointLight.h"
#include "Core/Graphics/Lighting/SpotLight.h"
#include "Core/Graphics/Drawable/Model.h"
#include "Core/Graphics/Drawable/Terrain.h"
#include "Core/Graphics/Drawable/Skybox.h"
#include "Core/Graphics/Drawable/ReflectionPlane.h"
#include "Core/Graphics/Drawable/SpriteManager.h"
#include "Core/Graphics/Drawable/Sprite.h"
#include "Core/Graphics/Lighting/DeferredLights.h"

namespace Kaka
{
	class Game
	{
	public:
		Game();
		// Game loop
		int Go();

	private:
		void LoadModelThreaded(const std::string& aModelPath, Model& aModel);
		void PointLightTest(float aDeltaTime);
		void Update(const float aDeltaTime);
		void HandleInput(const float aDeltaTime);
		void ShowStatsWindow();
		float GetDistanceBetweenObjects(DirectX::XMFLOAT3 aPosA, DirectX::XMFLOAT3 aPosB) const;

	private:
		ImGuiManager imGui;
		Window wnd;
		Timer timer;
		Camera camera;

		Camera directionalLightShadowCamera;

		DeferredLights deferredLights;

		//DirectionalLight directionalLight{wnd.Gfx(), 1u};

		std::vector<Sprite> sprites = {};
		std::vector<PointLight> pointLights = {};
		std::vector<SpotLight> spotLights = {};

		SpotLightData* flashLightTest = nullptr;
		SpotLightData* flashLightTest2 = nullptr;

		PointLightData* pointLightTest = nullptr;

		float flashLightBleedAngleMultiplier = 2.0f;
		float flashLightBleedIntensityFactor = 0.5f;
		float pointLightPositionOffsetFactor = 1.0f;
		float pointLightPositionInterpSpeed = 45.0f;
		float pointLightColourInterpSpeed = 5.0f;
		float pointLightIntensityInterpSpeed = 5.0f;
		float pointLightIntensity = 5.0f;
		float pointLightRadius = 50.0f;
		//float pointLightAngles[] = PI;
		//float pointLightAngles[4] = {0.0f,PI / 2,PI,PI * 1.5f};

	private:
		bool showImGui = true;
		bool showStatsWindow = true;
		bool drawLightDebug = false;

	private:
		float cameraSpeed = 0.0f;
		float cameraSpeedDefault = 3.0f;
		float cameraSpeedBoost = 6.0f;

	private:
		std::vector<Model> models;
		//Model vfxModel{};
		//Model vfxModelOlle{};
		//std::vector<Model> threadedModels{};

		//Terrain terrain{};
		Skybox skybox{};
		//ReflectionPlane reflectionPlane{};
		float skyboxSpeed = 0.005f;
		DirectX::XMFLOAT3 skyboxAngle = {};
		//std::vector<float> pointLightTravelRadiuses = {};
		//std::vector<float> pointLightTravelSpeeds = {};
		//std::vector<float> pointLightTravelAngles = {};

		//std::vector<float> spotLightTravelRadiuses = {};
		//std::vector<float> spotLightTravelSpeeds = {};
		//std::vector<float> spotLightTravelAngles = {};

		//std::vector<float> spriteTravelRadiuses = {};
		//std::vector<float> spriteTravelSpeeds = {};
		//std::vector<float> spriteTravelAngles = {};

		//struct ReflectionWaveBuffer
		//{
		//	DirectX::XMFLOAT2 k0;
		//	DirectX::XMFLOAT2 k1;
		//	float A;
		//	float padding[3];
		//};

		//ReflectionWaveBuffer reflectionPSBuffer = {};

		//struct ReflectionHeightBuffer
		//{
		//	float height;
		//	float padding[3];
		//};

		//ReflectionHeightBuffer reflectionVSBuffer = {};
		//ReflectionHeightBuffer reflectionHeightPSBuffer = {};

		struct CommonBuffer
		{
			DirectX::XMMATRIX worldToClipMatrix;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;
			DirectX::XMMATRIX clipToWorldMatrix;
			DirectX::XMMATRIX viewInverse;
			DirectX::XMFLOAT4 cameraPosition;
			DirectX::XMFLOAT2 resolution;
			float currentTime;
			float padding;
		};

		CommonBuffer commonBuffer = {};

		PostProcessing postProcessing;

		struct PostProcessingBuffer
		{
			DirectX::XMFLOAT3 tint; // RGB values for tint adjustment
			float exposure; // Exposure adjustment
			DirectX::XMFLOAT3 blackpoint; // Blackpoint adjustment
			float contrast; // Contrast adjustment
			float saturation; // Saturation adjustment
			float padding[3];
		};

		PostProcessingBuffer ppBuffer = {};

		std::thread updateThread;
		std::thread loadModelsThread;
		std::vector<std::thread> modelLoadingThreads;
		std::vector<bool> threadHasStarted = {};
		std::mutex modelLoadingMutex;
		float loadRadius = 100.0f;
		float cameraMoveSpeed = 10.0f;

		struct ShadowBuffer
		{
			BOOL usePCF = false;
			float offsetScalePCF = 0.004f;
			int sampleCountPCF = 5;
			BOOL usePoisson = true;
			float offsetScalePoissonDisk = 0.0009f;
			float padding[3];
		};

		ShadowBuffer shadowBuffer = {};

		struct RSMBuffer
		{
			BOOL useRSM = true;
			BOOL onlyRSM = false;
			float rMax = 0.05f; // Maximum sampling radius.
			float rsmIntensity = 20.0f;
		};

		RSMBuffer rsmBuffer = {};
	};
}
