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
#include "Graphics/GraphicsConstants.h"

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

		std::vector<Sprite> sprites = {};
		std::vector<PointLight> pointLights = {};
		std::vector<SpotLight> spotLights = {};

		SpotLightData* flashLightTest = nullptr;
		SpotLightData* flashLightTest2 = nullptr;

		//PointLightData* pointLightTest = nullptr;

		float flashLightBleedAngleMultiplier = 2.0f;
		float flashLightBleedIntensityFactor = 0.5f;
		float pointLightPositionOffsetFactor = 1.0f;
		float pointLightPositionInterpSpeed = 45.0f;
		float pointLightColourInterpSpeed = 5.0f;
		float pointLightIntensityInterpSpeed = 5.0f;
		float pointLightIntensity = 5.0f;
		float pointLightRadius = 50.0f;

	private:
		bool showImGui = true;
		bool showStatsWindow = true;
		bool drawLightDebug = false;

	private:
		DirectX::XMFLOAT3 cameraInput = {0.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 cameraVelocity = {0.0f, 0.0f, 0.0f};
		float cameraMoveInterpSpeed = 10.0f;
		float cameraRotateInterpSpeed = 25.0f;
		float cameraSpeed = 0.0f;
		float cameraSpeedDefault = 3.0f;
		float cameraSpeedBoost = 6.0f;

	private:
		std::vector<Model> models;
		Texture* whiteTexture = nullptr;
		Texture* blueTexture = nullptr;
		Texture* redTexture = nullptr;

		Skybox skybox{};
		float skyboxSpeed = 0.005f;
		DirectX::XMFLOAT3 skyboxAngle = {};

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
			BOOL usePoisson = false;
			unsigned int sampleCount = 100;
			float rMax = 0.08f; // Maximum sampling radius.
			float rsmIntensity = 10.0f;
			float padding[2];
			DirectX::XMFLOAT4 shadowColour = {0.8f, 0.9f, 1.0f, 0.01f};
			DirectX::XMFLOAT4 ambianceColour = {0.1f, 0.1f, 0.1f, 0.85f};
			//float shadowIntensity = 0.25f;
		};

		RSMBuffer rsmBuffer = {};

		struct RSMLightData
		{
			DirectX::XMFLOAT4 lightColourAndIntensity;
			float falloff;
			BOOL isDirectionalLight;
			float padding[2];
		} rsmLightData;

		PixelConstantBuffer<CommonBuffer> pcb{wnd.Gfx(), PS_CBUFFER_SLOT_COMMON};
		VertexConstantBuffer<CommonBuffer> vcb{wnd.Gfx(), VS_CBUFFER_SLOT_COMMON};
		PixelConstantBuffer<RSMLightData> rsmLightDataBuffer{wnd.Gfx(), PS_CBUFFER_SLOT_RSM_LIGHT};
	};
}
