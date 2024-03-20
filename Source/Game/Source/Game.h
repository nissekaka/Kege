#pragma once
#include "Core/Windows/Window.h"
#include "Core/Utility/ImGuiManager.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/Camera.h"
#include <Core/Utility/KakaMath.h>

#include "Core/Graphics/PostProcessing/PostProcessing.h"
#include "Core/Graphics/Lighting/DirectionalLight.h"
#include "Core/Graphics/Lighting/Pointlight.h"
#include "Core/Graphics/Lighting/Spotlight.h"
#include "Core/Graphics/Lighting/IndirectLighting.h"
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
		void Update(const float aDeltaTime);
		void HandleInput(const float aDeltaTime);
		void ShowStatsWindow();
		float GetDistanceBetweenObjects(DirectX::XMFLOAT3 aPosA, DirectX::XMFLOAT3 aPosB) const;

	private:
		ImGuiManager imGui;
		Window wnd;
		Timer timer;
		Camera camera;

		DeferredLights deferredLights;

		std::vector<Sprite> sprites = {};
		std::vector<Pointlight> pointlights = {};
		std::vector<Spotlight> spotlights = {};

		SpotlightData* flashlightInner = nullptr;
		SpotlightData* flashlightOuter = nullptr;

		float flashlightPositionInterpSpeed = 15.0f;
		float flashlightDirectionInterpSpeed = 5.0f;
		float flashlightBleedAngleMultiplier = 1.9f;
		float flashlightBleedIntensityFactor = 0.05f;
		float flashlightIntensityInterpSpeed = 20.0f;
		float flashlightIntensity;
		bool flashlightOn = true;

		Texture* flashlightTexture;

		Sprite dustParticles;
		Sprite smokeParticles;

	private:
		bool showImGui = true;
		bool showStatsWindow = true;
		bool drawLightDebug = false;
		bool drawRSM = true;

	private:
		DirectX::XMFLOAT3 cameraInput = {0.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 cameraVelocity = {0.0f, 0.0f, 0.0f};
		float cameraMoveInterpSpeed = 8.0f;
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
		IndirectLighting indirectLighting;

		struct PostProcessingBuffer
		{
			DirectX::XMFLOAT3 tint; // RGB values for tint adjustment
			float exposure; // Exposure adjustment
			DirectX::XMFLOAT3 blackpoint; // Blackpoint adjustment
			float contrast; // Contrast adjustment
			float saturation; // Saturation adjustment
			float blur; // Blur adjustment
			float padding[2];
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
			float offsetScalePoissonDisk = 0.0019f;
			float padding[3];
		};

		ShadowBuffer shadowBuffer = {};

		enum class eRSMMode
		{
			NoAlbedo,
			WithAlbedo,
			Debug,
			Count
		};

		std::string ModeEnumToString[3]
		{
			"No albedo",
			"With albedo",
			"Debug"
		};

		struct RSMBuffer
		{
			BOOL usePoisson = false;
			BOOL isDirectionalLight = true;
			UINT mode = 0u;
			UINT sampleCount = 100u;
			UINT sampleCountLastPass = 100u;
			UINT currentPass = 0;
			UINT type = 0;
			float rMax = 0.08f; // Maximum sampling radius.
			float rsmIntensity = 10.0f;
			float uvScale = 1;
			float weightMax = 0.1f;
			float divideN = 1.0f;
			float divideP = 20.0f;
			float padding[3] = {};
			DirectX::XMFLOAT4 shadowColour = {0.8f, 0.9f, 1.0f, 0.01f};
			DirectX::XMFLOAT4 ambianceColour = {0.8f, 0.9f, 1.0f, 0.3f};
			DirectX::XMMATRIX lightCameraTransform;
		};

		static constexpr int RSM_MAX_SAMPLE_COUNT = 5000;

		RSMBuffer rsmBufferDirectional = {};
		RSMBuffer rsmBufferSpot = {};

		struct RSMLightData
		{
			float colourAndIntensity[4];
			float directionAndInnerAngle[4];
			float lightPositionAndOuterAngle[4];
			float range;
			BOOL isDirectionalLight;
			float padding[2];
		} rsmLightData;

		struct RSMCombinedBuffer
		{
			UINT currentPass = 0;
			float padding[3];
		} rsmCombinedBuffer;

		int combinedPasses = 4;

		PixelConstantBuffer<CommonBuffer> pcb{wnd.Gfx(), PS_CBUFFER_SLOT_COMMON};
		VertexConstantBuffer<CommonBuffer> vcb{wnd.Gfx(), VS_CBUFFER_SLOT_COMMON};
		PixelConstantBuffer<RSMLightData> rsmLightDataBuffer{wnd.Gfx(), PS_CBUFFER_SLOT_RSM_LIGHT};

		//ID3D11ShaderResourceView* hammersleySRV = nullptr;

		static constexpr unsigned int HAMMERSLEY_DIR_COUNT = 4u;
		static constexpr unsigned int HAMMERSLEY_SPOT_COUNT = 2u;
		static constexpr unsigned int HAMMERSLEY_FINAL_COUNT = 0u;

		struct TAABuffer
		{
			unsigned int mode = 0u;
			float denoise = 3.0f;
			float padding[2];
		} taaBuffer;

		bool flipFlop = false;

		//struct HammerDataDirectional
		//{
		//	float x[HAMMERSLEY_DIR_COUNT] = {};
		//	float y[HAMMERSLEY_DIR_COUNT] = {};
		//} hammerDataDirectional;

		//struct HammerDataSpot
		//{
		//	float x[HAMMERSLEY_SPOT_COUNT] = {};
		//	float y[HAMMERSLEY_SPOT_COUNT] = {};
		//} hammerDataSpot;

		//struct HammerDataFinal
		//{
		//	float x[HAMMERSLEY_FINAL_COUNT] = {};
		//	float y[HAMMERSLEY_FINAL_COUNT] = {};
		//} hammerDataFinal;

		//struct HammersleyData
		//{
		//	DirectX::XMFLOAT2 pointsDir[2000] = {}; // Bytes = 16000
		//	DirectX::XMFLOAT2 pointsSpot[256] = {}; // Bytes = 18048
		//	DirectX::XMFLOAT2 pointsFinal[64] = {}; // Bytes = 18560
		//	unsigned int dirCount = {}; // 18564
		//	unsigned int spotCount = {}; // 18568
		//	unsigned int finalCount = {}; // 18572
		//	float padding = {};
		//} hData;

		//PixelConstantBuffer<HammerDataDirectional> hammersleyDirectionalBuffer{wnd.Gfx(), 5u};
		//PixelConstantBuffer<HammerDataSpot> hammersleySpotBuffer{wnd.Gfx(), 6u};
		//PixelConstantBuffer<HammerDataFinal> hammersleyFinalBuffer{wnd.Gfx(), 7u};
	};
}
