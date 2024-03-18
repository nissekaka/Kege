#include "Game.h"
#include <Core/Utility/KakaMath.h>
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>
#include <future>
#include <random>
#include <TGAFBXImporter/source/Internal.inl>

#include "Graphics/GraphicsConstants.h"
#include "Graphics/Drawable/ModelLoader.h"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int NUM_POINT_LIGHTS = 0;
constexpr int NUM_SPOT_LIGHTS = 0;
constexpr int TERRAIN_SIZE = 0;

constexpr int MODELS_TO_LOAD_THREADED = 10;
constexpr float SHADOW_RESOLUTION_DIVIDER = 4.0f;

// Reverses the bits of the input
unsigned int BitfieldReverse(const unsigned int aBits)
{
	unsigned int b = (unsigned int(aBits) << 16u) | (unsigned int(aBits) >> 16u);
	b = (b & 0x55555555u) << 1u | (b & 0xAAAAAAAAu) >> 1u;
	b = (b & 0x33333333u) << 2u | (b & 0xCCCCCCCCu) >> 2u;
	b = (b & 0x0F0F0F0Fu) << 4u | (b & 0xF0F0F0F0u) >> 4u;
	b = (b & 0x00FF00FFu) << 8u | (b & 0xFF00FF00u) >> 8u;
	return b;
}

DirectX::XMFLOAT2 Hammersley(const unsigned int aI, const unsigned int aN)
{
	return DirectX::XMFLOAT2(float(aI) / float(aN), float(BitfieldReverse(aI)) * 2.3283064365386963e-10);
}

namespace Kaka
{
	Game::Game()
		:
		wnd(WINDOW_WIDTH, WINDOW_HEIGHT, L"Kaka")
	{
		camera.SetPerspective(WINDOW_WIDTH, WINDOW_HEIGHT, 80, 0.5f, 5000.0f);
		wnd.Gfx().directionalLightRSMBuffer.GetCamera().SetOrthographic(WINDOW_WIDTH / 3.0f, WINDOW_HEIGHT / 3.0f, -500.0f, 500.0f);

		for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			pointlights.emplace_back(Pointlight{wnd.Gfx(), 2u});
		}

		for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
		{
			spotlights.emplace_back(Spotlight{wnd.Gfx(), 3u});
		}

		//threadedModels.resize(MODELS_TO_LOAD_THREADED);
		//modelLoadingThreads.resize(threadedModels.size());
		for (int i = 0; i < modelLoadingThreads.size(); ++i)
		{
			threadHasStarted.push_back(false);
		}

		deferredLights.Init(wnd.Gfx());
	}

	int Game::Go()
	{
		postProcessing.Init(wnd.Gfx());
		indirectLighting.Init(wnd.Gfx());

		ppBuffer.tint = {1.0f, 1.0f, 1.0f};
		ppBuffer.blackpoint = {0.0f, 0.0f, 0.0f};
		ppBuffer.exposure = 0.0f;
		ppBuffer.contrast = 1.0f;
		ppBuffer.saturation = 1.0f;

		skybox.Init(wnd.Gfx(), "Assets\\Textures\\Skybox\\Miramar\\", "Assets\\Textures\\Skybox\\Kurt\\");

		wnd.Gfx().directionalLightRSMBuffer.GetCamera().SetPosition({0.0f, 70.0f, 0.0f});
		camera.SetPosition({-11.0f, 28.0f, 26.0f});
		camera.SetRotationDegrees(29.0f, 138.0f);

		models.emplace_back();
		models.back().LoadModel(wnd.Gfx(), "Assets\\Models\\sponza_pbr\\Sponza.obj", Model::eShaderType::PBR);
		//models.back().LoadModel(wnd.Gfx(), "Assets\\Models\\NewSponza\\New_Sponza_001.gltf", Model::eShaderType::PBR);
		models.back().Init();
		models.back().SetScale(0.1f);

		flashlightTexture = ModelLoader::LoadTexture(wnd.Gfx(), "Assets\\Textures\\Flashlight_cookie.png", 5u);
		// 524288 * 4 = 2 097 152
		// 524288 * 2 = 1 048 576
		dustParticles.Init(wnd.Gfx(), 0.0125f, 1048576u, true, "Assets\\Textures\\particle.png");
		//smokeParticles.Init(wnd.Gfx(), 25.0f, 1000u, true, "Assets\\Textures\\SpriteCloud.png");

		// Flashlight setup
		{
			flashlightInner = &deferredLights.AddSpotLight();
			flashlightInner->intensity = 5000.0f;
			flashlightIntensity = flashlightInner->intensity;
			flashlightInner->range = 1000.0f;
			flashlightInner->innerAngle = 0.13f; // Radians
			flashlightInner->outerAngle = 0.32f; // Radians
			flashlightInner->colour = {1.0f, 0.9f, 0.6f};
			flashlightInner->useTexture = TRUE;
			flashlightInner->useVolumetricLight = TRUE;
			flashlightInner->numberOfVolumetricSteps = 15u;
			flashlightInner->volumetricScattering = 0.12f;
			flashlightInner->volumetricIntensity = 3.0f;
			flashlightInner->volumetricAngle = DegToRad(5.0f);
			flashlightInner->volumetricRange = 30.0f;
			flashlightInner->volumetricFade = 25.0f;
			flashlightInner->volumetricAlpha = 0.5f;

			wnd.Gfx().spotLightRSMBuffer[0].GetCamera().SetPerspective(WINDOW_WIDTH, WINDOW_HEIGHT, 120.0f, 0.5f, 5000.0f);

			flashlightOuter = &deferredLights.AddSpotLight();
			flashlightOuter->intensity = flashlightInner->intensity * 0.5f;
			flashlightOuter->range = flashlightInner->range;
			flashlightOuter->innerAngle = flashlightInner->innerAngle; // Radians
			flashlightOuter->outerAngle = std::clamp(flashlightInner->outerAngle * 10.0f, flashlightOuter->outerAngle, 3.14f); // Radians
			flashlightOuter->colour = flashlightInner->colour;
			flashlightOuter->useTexture = FALSE;
			flashlightOuter->useVolumetricLight = TRUE;
			flashlightOuter->numberOfVolumetricSteps = 15u;
			flashlightOuter->volumetricScattering = 0.12f;
			flashlightOuter->volumetricIntensity = 0.5f;
			flashlightOuter->volumetricAngle = DegToRad(15.0f);
			flashlightOuter->volumetricRange = 30.0f;
			flashlightOuter->volumetricFade = 25.0f;
			flashlightOuter->volumetricAlpha = 0.2f;
		}

		rsmBufferDirectional.type = 0u;
		rsmBufferDirectional.sampleCount = HAMMERSLEY_DIR_COUNT;
		rsmBufferDirectional.sampleCountLastPass = HAMMERSLEY_FINAL_COUNT;
		rsmBufferDirectional.rMax = 0.11f;
		rsmBufferDirectional.rsmIntensity = 15000.0f;
		rsmBufferDirectional.uvScale = wnd.Gfx().rsmDownscaleFactor;
		rsmBufferDirectional.isDirectionalLight = TRUE;
		rsmBufferDirectional.weightMax = 3.7f;

		rsmBufferSpot.type = 1u;
		rsmBufferSpot.sampleCount = HAMMERSLEY_SPOT_COUNT;
		rsmBufferSpot.sampleCountLastPass = HAMMERSLEY_FINAL_COUNT;
		rsmBufferSpot.rMax = 0.165f;
		rsmBufferSpot.rsmIntensity = 100.0f;
		rsmBufferSpot.uvScale = wnd.Gfx().rsmDownscaleFactor;
		rsmBufferSpot.isDirectionalLight = FALSE;
		rsmBufferSpot.weightMax = 3.8f;


		//// Set up compute shader resources
		//ID3D11Buffer* hammersleyBuffer;
		//// Create the buffer to store Hammersley sequence, the size should be equal to sample count
		//{
		//	D3D11_BUFFER_DESC bufferDesc = {};
		//	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT2) * 600;
		//	bufferDesc.CPUAccessFlags = 0;
		//	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		//	bufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT2);
		//	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

		//	wnd.Gfx().pDevice->CreateBuffer(&bufferDesc, nullptr, &hammersleyBuffer);
		//}

		//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		//srvDesc.Format = DXGI_FORMAT_R32G32_FLOAT;;
		//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		//srvDesc.Buffer.FirstElement = 0;
		//srvDesc.Buffer.NumElements = 600;
		//srvDesc.Buffer.ElementWidth = sizeof(DirectX::XMFLOAT2);
		//srvDesc.Buffer.ElementOffset = 0;
		//wnd.Gfx().pDevice->CreateShaderResourceView(hammersleyBuffer, &srvDesc, &hammersleySRV);

		//// Set the compute shader resources
		//wnd.Gfx().pContext->CSSetShaderResources(0, 1, &hammersleySRV);

		//ComputeShader* computeShader = ShaderFactory::GetComputeShader(wnd.Gfx(), L"Shaders\\RSMSampling_CS.cso");

		//// Dispatch the compute shader
		//computeShader->Bind(wnd.Gfx());
		//wnd.Gfx().pContext->Dispatch(600, 1, 1);

		//// Unbind resources after dispatch
		//ID3D11ShaderResourceView* nullSRV = nullptr;
		//wnd.Gfx().pContext->CSSetShaderResources(0, 1, &nullSRV);

		//hData.dirCount = 600;
		//hData.spotCount = 256;
		//hData.finalCount = 64;

		//for (int i = 0; i < hData.dirCount; ++i)
		//{
		//	hData.pointsDir[i] = (Hammersley(i, hData.dirCount));
		//}

		//for (int i = 0; i < hData.spotCount; ++i)
		//{
		//	hData.pointsSpot[i] = (Hammersley(i, hData.spotCount));
		//}

		//for (int i = 0; i < hData.finalCount; ++i)
		//{
		//	hData.pointsFinal[i] = (Hammersley(i, hData.finalCount));
		//}

		for (int i = 0; i < HAMMERSLEY_DIR_COUNT; ++i)
		{
			const DirectX::XMFLOAT2 point = Hammersley(i, HAMMERSLEY_DIR_COUNT);
			hammerDataDirectional.x[i] = point.x;
			hammerDataDirectional.y[i] = point.y;
		}

		for (int i = 0; i < HAMMERSLEY_SPOT_COUNT; ++i)
		{
			const DirectX::XMFLOAT2 point = Hammersley(i, HAMMERSLEY_SPOT_COUNT);
			hammerDataSpot.x[i] = point.x;
			hammerDataSpot.y[i] = point.y;
		}

		for (int i = 0; i < HAMMERSLEY_FINAL_COUNT; ++i)
		{
			const DirectX::XMFLOAT2 point = Hammersley(i, HAMMERSLEY_FINAL_COUNT);
			hammerDataFinal.x[i] = point.x;
			hammerDataFinal.y[i] = point.y;
		}

		hammersleyDirectionalBuffer.Update(wnd.Gfx(), hammerDataDirectional);
		hammersleySpotBuffer.Update(wnd.Gfx(), hammerDataSpot);
		hammersleyFinalBuffer.Update(wnd.Gfx(), hammerDataFinal);

		while (true)
		{
			// Process all messages pending
			if (const auto code = Window::ProcessMessages())
			{
				// If return optional has value, we're quitting
				return *code;
			}
			// If no value
			Update(timer.UpdateDeltaTime());
		}
	}

	void Game::LoadModelThreaded(const std::string& aModelPath, Model& aModel)
	{
		modelLoadingMutex.lock();
		aModel.LoadModel(wnd.Gfx(), aModelPath, Model::eShaderType::AnimPBR);
		aModel.Init();
		modelLoadingMutex.unlock();
	}

	void Game::Update(const float aDeltaTime)
	{
		UNREFERENCED_PARAMETER(aDeltaTime);

		//for (int i = 0; i < threadedModels.size(); ++i)
		//{
		//	const float distance = GetDistanceBetweenObjects(camera.GetPosition(), threadedModels[i].GetPosition());

		//	if (distance < loadRadius)
		//	{
		//		// We only want to start the thread if it hasn't already been started
		//		if (!threadHasStarted[i])
		//		{
		//			threadHasStarted[i] = true;
		//			std::string path[3] = {
		//				"Assets\\Models\\player\\sk_player.fbx",
		//				"Assets\\Models\\crawler\\CH_NPC_Crawler_01_22G3S_SK.fbx",
		//				"Assets\\Models\\wizard\\SM_wizard.fbx",
		//			};
		//			modelLoadingThreads[i] = std::thread(&Game::LoadModelThreaded, this, path[i % 3], std::ref(threadedModels[i]));

		//			// We don't have to wait for this thread, it will just do its thing
		//			// So we detach	it
		//			modelLoadingThreads[i].detach();
		//		}
		//	}
		//}

		//camera.SetPosition({camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z - cameraMoveSpeed * aDeltaTime});


		// Begin frame
		wnd.Gfx().BeginFrame();
		wnd.Gfx().SetCamera(camera);

		HandleInput(aDeltaTime);

		// Flashlight
		{
			if (flashlightOn)
			{
				if (flashlightInner->intensity < flashlightIntensity)
				{
					flashlightInner->intensity = Interp(flashlightInner->intensity, flashlightIntensity, aDeltaTime * flashlightIntensityInterpSpeed);
				}
			}
			else
			{
				if (flashlightInner->intensity > 0.0f)
				{
					flashlightInner->intensity = Interp(flashlightInner->intensity, 0.0f, aDeltaTime * flashlightIntensityInterpSpeed);
				}
			}

			// Move flashlight a bit from the camera so that we see the shadows

			// Forward vector
			constexpr float forwardOffsetFactor = 2.5f;

			DirectX::XMFLOAT3 targetPosition = camera.GetPosition();
			DirectX::XMFLOAT3 targetDirection;
			DirectX::XMStoreFloat3(&targetDirection, camera.GetForwardVector());

			flashlightInner->direction = targetDirection;
			flashlightInner->direction = {targetDirection.x * -1.0f, targetDirection.y * -1.0f, targetDirection.z * -1.0f};

			targetPosition.x += targetDirection.x * forwardOffsetFactor;
			targetPosition.y += targetDirection.y * forwardOffsetFactor;
			targetPosition.z += targetDirection.z * forwardOffsetFactor;

			// Right vector
			constexpr float rightOffsetFactor = 2.5f;

			DirectX::XMFLOAT3 right;
			DirectX::XMStoreFloat3(&right, camera.GetRightVector());
			targetPosition.x += right.x * rightOffsetFactor;
			targetPosition.y += right.y * rightOffsetFactor;
			targetPosition.z += right.z * rightOffsetFactor;

			// Up vector
			constexpr float upOffsetFactor = 2.5f;
			DirectX::XMFLOAT3 up;
			DirectX::XMStoreFloat3(&up, camera.GetUpVector());
			targetPosition.x -= up.x * upOffsetFactor;
			targetPosition.y -= up.y * upOffsetFactor;
			targetPosition.z -= up.z * upOffsetFactor;

			flashlightInner->position.x = Interp(flashlightInner->position.x, targetPosition.x, aDeltaTime * flashlightPositionInterpSpeed);
			flashlightInner->position.y = Interp(flashlightInner->position.y, targetPosition.y, aDeltaTime * flashlightPositionInterpSpeed);
			flashlightInner->position.z = Interp(flashlightInner->position.z, targetPosition.z, aDeltaTime * flashlightPositionInterpSpeed);

			wnd.Gfx().spotLightRSMBuffer[0].GetCamera().SetPosition(flashlightInner->position);

			flashlightOuter->position = flashlightInner->position;
			flashlightOuter->direction = flashlightInner->direction;
			flashlightOuter->intensity = flashlightInner->intensity * flashlightBleedIntensityFactor;
			flashlightOuter->range = flashlightInner->range;
			flashlightOuter->innerAngle = flashlightInner->innerAngle; // Radians
			flashlightOuter->outerAngle = std::clamp(flashlightInner->outerAngle * flashlightBleedAngleMultiplier, flashlightInner->outerAngle, 3.14f); // Radians
			flashlightOuter->colour = flashlightInner->colour;
		}

		commonBuffer.worldToClipMatrix = camera.GetInverseView() * camera.GetProjection();
		commonBuffer.view = camera.GetView();
		commonBuffer.projection = camera.GetProjection();
		commonBuffer.viewInverse = camera.GetInverseView();
		commonBuffer.clipToWorldMatrix = DirectX::XMMatrixInverse(nullptr, commonBuffer.worldToClipMatrix);
		commonBuffer.cameraPosition = {camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 0.0f};
		commonBuffer.resolution = wnd.Gfx().GetCurrentResolution();
		commonBuffer.currentTime = timer.GetTotalTime();

		pcb.Update(wnd.Gfx(), commonBuffer);
		pcb.Bind(wnd.Gfx());

		vcb.Update(wnd.Gfx(), commonBuffer);
		vcb.Bind(wnd.Gfx());

		skyboxAngle.y += skyboxSpeed * aDeltaTime;
		skybox.Rotate(skyboxAngle);

		dustParticles.Update(wnd.Gfx(), aDeltaTime);
		//smokeParticles.Update(wnd.Gfx(), aDeltaTime);

		wnd.Gfx().SetDepthStencilState(eDepthStencilStates::Normal);
		// Need backface culling for Reflective Shadow Maps
		wnd.Gfx().SetRasterizerState(eRasterizerStates::BackfaceCulling);

		// ---------- SHADOW MAP PASS -- DIRECTIONAL LIGHT ---------- BEGIN
		{
			wnd.Gfx().StartShadows(wnd.Gfx().directionalLightRSMBuffer.GetCamera(), deferredLights.GetDirectionalLightData().lightDirection, wnd.Gfx().directionalLightRSMBuffer, PS_TEXTURE_SLOT_SHADOW_MAP_DIRECTIONAL);
			deferredLights.SetShadowCamera(wnd.Gfx().directionalLightRSMBuffer.GetCamera().GetInverseView() * wnd.Gfx().directionalLightRSMBuffer.GetCamera().GetProjection());
			deferredLights.SetSpotLightShadowCamera(wnd.Gfx().spotLightRSMBuffer[0].GetCamera().GetInverseView() * wnd.Gfx().spotLightRSMBuffer[0].GetCamera().GetProjection(), 0);

			wnd.Gfx().directionalLightRSMBuffer.ClearTextures(wnd.Gfx().pContext.Get());
			wnd.Gfx().directionalLightRSMBuffer.SetAsActiveTarget(wnd.Gfx().pContext.Get());


			rsmLightData.colourAndIntensity[0] = deferredLights.GetDirectionalLightData().lightColour.x;
			rsmLightData.colourAndIntensity[1] = deferredLights.GetDirectionalLightData().lightColour.y;
			rsmLightData.colourAndIntensity[2] = deferredLights.GetDirectionalLightData().lightColour.z;
			rsmLightData.colourAndIntensity[3] = deferredLights.GetDirectionalLightData().lightIntensity;
			rsmLightData.isDirectionalLight = TRUE;

			rsmLightDataBuffer.Update(wnd.Gfx(), rsmLightData);
			rsmLightDataBuffer.Bind(wnd.Gfx());

			// Render everything that casts shadows
			{
				//terrain.Draw(wnd.Gfx());
				for (Model& model : models)
				{
					model.Draw(wnd.Gfx(), aDeltaTime);
				}
			}
		}
		// ---------- SHADOW MAP PASS -- DIRECTIONAL LIGHT ---------- END

		// ---------- SHADOW MAP PASS -- SPOT LIGHT ---------- BEGIN
		{
			DirectX::XMFLOAT3 direction = deferredLights.GetSpotLightData(0).direction;
			direction = {direction.x, direction.y * -1.0f, direction.z * -1.0f};

			Camera& shadowCamera = wnd.Gfx().spotLightRSMBuffer[0].GetCamera();
			const DirectX::XMMATRIX spotlightInverseViewProj = shadowCamera.GetInverseView() * shadowCamera.GetProjection();

			wnd.Gfx().StartShadows(shadowCamera, direction, wnd.Gfx().spotLightRSMBuffer[0], PS_TEXTURE_SLOT_SHADOW_MAP_SPOT);
			deferredLights.SetSpotLightShadowCamera(spotlightInverseViewProj, 0);
			deferredLights.SetSpotLightShadowCamera(spotlightInverseViewProj, 1);

			wnd.Gfx().spotLightRSMBuffer[0].ClearTextures(wnd.Gfx().pContext.Get());
			wnd.Gfx().spotLightRSMBuffer[0].SetAsActiveTarget(wnd.Gfx().pContext.Get());

			rsmLightData.colourAndIntensity[0] = deferredLights.GetSpotLightData(0).colour.x;
			rsmLightData.colourAndIntensity[1] = deferredLights.GetSpotLightData(0).colour.y;
			rsmLightData.colourAndIntensity[2] = deferredLights.GetSpotLightData(0).colour.z;
			rsmLightData.colourAndIntensity[3] = deferredLights.GetSpotLightData(0).intensity;
			rsmLightData.directionAndInnerAngle[0] = deferredLights.GetSpotLightData(0).direction.x;
			rsmLightData.directionAndInnerAngle[1] = deferredLights.GetSpotLightData(0).direction.y;
			rsmLightData.directionAndInnerAngle[2] = deferredLights.GetSpotLightData(0).direction.z;
			rsmLightData.directionAndInnerAngle[3] = deferredLights.GetSpotLightData(0).innerAngle;
			rsmLightData.lightPositionAndOuterAngle[0] = deferredLights.GetSpotLightData(0).position.x;
			rsmLightData.lightPositionAndOuterAngle[1] = deferredLights.GetSpotLightData(0).position.y;
			rsmLightData.lightPositionAndOuterAngle[2] = deferredLights.GetSpotLightData(0).position.z;
			rsmLightData.lightPositionAndOuterAngle[3] = deferredLights.GetSpotLightData(0).outerAngle;
			rsmLightData.range = deferredLights.GetSpotLightData(0).range;
			rsmLightData.isDirectionalLight = FALSE;

			rsmLightDataBuffer.Update(wnd.Gfx(), rsmLightData);
			rsmLightDataBuffer.Bind(wnd.Gfx());

			// Render everything that casts shadows
			{
				for (Model& model : models)
				{
					model.Draw(wnd.Gfx(), aDeltaTime);
				}
			}

			wnd.Gfx().ResetShadows(camera);
		}
		// ---------- SHADOW MAP PASS -- SPOT LIGHT ---------- END


		// GBuffer pass -- BEGIN
		{
			wnd.Gfx().gBuffer.ClearTextures(wnd.Gfx().pContext.Get());
			wnd.Gfx().gBuffer.SetAsActiveTarget(wnd.Gfx().pContext.Get(), wnd.Gfx().gBuffer.GetDepthStencilView());

			for (Model& model : models)
			{
				model.Draw(wnd.Gfx(), aDeltaTime);
			}

			wnd.Gfx().SetRenderTarget(eRenderTargetType::None, nullptr);
			wnd.Gfx().gBuffer.SetAllAsResources(wnd.Gfx().pContext.Get(), PS_GBUFFER_SLOT);
		}
		// GBuffer pass -- END

		//// Bind the hammersley shader resource view to slot 11
		//wnd.Gfx().pContext->PSSetShaderResources(11, 1, &hammersleySRV);

		// Indirect lighting pass -- BEGIN
		{
			if (drawRSM)
			{
				//hammersleyDirectionalBuffer.Bind(wnd.Gfx());
				//hammersleyFinalBuffer.Bind(wnd.Gfx());
				//hammersleySpotBuffer.Bind(wnd.Gfx());

				// Directional light
				wnd.Gfx().directionalLightRSMBuffer.SetAllAsResources(wnd.Gfx().pContext.Get(), PS_RSM_SLOT_DIRECTIONAL);
				rsmBufferDirectional.lightCameraTransform = wnd.Gfx().directionalLightRSMBuffer.GetCamera().GetInverseView() * wnd.Gfx().directionalLightRSMBuffer.GetCamera().GetProjection();
				PixelConstantBuffer<RSMBuffer> rsmPixelBuffer{wnd.Gfx(), PS_CBUFFER_SLOT_RSM_DIRECTIONAL};

				for (int i = 0; i < combinedPasses; ++i)
				{
					if (i == 0)
					{
						wnd.Gfx().SetRenderTarget(eRenderTargetType::RSMDownscaleDirectional, nullptr);
					}
					else
					{
						wnd.Gfx().SetRenderTarget(eRenderTargetType::RSMFullscaleDirectional, nullptr);
						wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTRUE_SLOT_INDIRECT_LIGHT_DIRECTIONAL, 1u, wnd.Gfx().rsmDownscaleDirectional.pResource.GetAddressOf());
					}

					rsmBufferDirectional.currentPass = i;
					rsmBufferDirectional.isDirectionalLight = TRUE;
					rsmPixelBuffer.Update(wnd.Gfx(), rsmBufferDirectional);
					rsmPixelBuffer.Bind(wnd.Gfx());

					indirectLighting.Draw(wnd.Gfx());
				}

				wnd.Gfx().directionalLightRSMBuffer.ClearAllAsResourcesSlots(wnd.Gfx().pContext.Get(), PS_RSM_SLOT_DIRECTIONAL);

				// Spot light
				wnd.Gfx().spotLightRSMBuffer[0].SetAllAsResources(wnd.Gfx().pContext.Get(), PS_RSM_SLOT_DIRECTIONAL);
				rsmBufferSpot.lightCameraTransform = wnd.Gfx().spotLightRSMBuffer[0].GetCamera().GetInverseView() * wnd.Gfx().spotLightRSMBuffer[0].GetCamera().GetProjection();

				for (int i = 0; i < combinedPasses; ++i)
				{
					if (i == 0)
					{
						wnd.Gfx().SetRenderTarget(eRenderTargetType::RSMDownscaleSpot, nullptr);
					}
					else
					{
						wnd.Gfx().SetRenderTarget(eRenderTargetType::RSMFullscaleSpot, nullptr);
						wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTURE_SLOT_INDIRECT_LIGHT_SPOT, 1u, wnd.Gfx().rsmDownscaleSpot.pResource.GetAddressOf());
					}

					rsmBufferSpot.currentPass = i;
					rsmBufferSpot.isDirectionalLight = FALSE;
					rsmPixelBuffer.Update(wnd.Gfx(), rsmBufferSpot);
					rsmPixelBuffer.Bind(wnd.Gfx());

					indirectLighting.Draw(wnd.Gfx());
				}

				wnd.Gfx().spotLightRSMBuffer[0].ClearAllAsResourcesSlots(wnd.Gfx().pContext.Get(), PS_RSM_SLOT_DIRECTIONAL);
			}
		}
		// Indirect lighting pass -- END

		// Lighting pass
		{
			wnd.Gfx().SetRenderTarget(eRenderTargetType::PostProcessing, nullptr);

			PixelConstantBuffer<ShadowBuffer> shadowPixelBuffer{wnd.Gfx(), PS_CBUFFER_SLOT_SHADOW};
			shadowPixelBuffer.Update(wnd.Gfx(), shadowBuffer);
			shadowPixelBuffer.Bind(wnd.Gfx());

			flashlightTexture->Bind(wnd.Gfx());

			wnd.Gfx().BindShadows(wnd.Gfx().directionalLightRSMBuffer, PS_TEXTURE_SLOT_SHADOW_MAP_DIRECTIONAL);
			wnd.Gfx().BindShadows(wnd.Gfx().spotLightRSMBuffer[0], PS_TEXTURE_SLOT_SHADOW_MAP_SPOT);
			deferredLights.Draw(wnd.Gfx());
			wnd.Gfx().UnbindShadows(PS_TEXTURE_SLOT_SHADOW_MAP_DIRECTIONAL);
			wnd.Gfx().UnbindShadows(PS_TEXTURE_SLOT_SHADOW_MAP_SPOT);
		}

		// Indirect combined pass -- BEGIN

		if (drawRSM)
		{
			wnd.Gfx().SetBlendState(eBlendStates::Additive);

			if (combinedPasses > 1)
			{
				wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTRUE_SLOT_INDIRECT_LIGHT_DIRECTIONAL, 1u, wnd.Gfx().rsmFullscaleDirectional.pResource.GetAddressOf());
				wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTURE_SLOT_INDIRECT_LIGHT_SPOT, 1u, wnd.Gfx().rsmFullscaleSpot.pResource.GetAddressOf());
			}
			else
			{
				wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTRUE_SLOT_INDIRECT_LIGHT_DIRECTIONAL, 1u, wnd.Gfx().rsmDownscaleDirectional.pResource.GetAddressOf());
				wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTURE_SLOT_INDIRECT_LIGHT_SPOT, 1u, wnd.Gfx().rsmDownscaleSpot.pResource.GetAddressOf());
			}

			indirectLighting.SetPixelShaderCombined(true);

			indirectLighting.Draw(wnd.Gfx());

			indirectLighting.SetPixelShaderCombined(false);
			wnd.Gfx().SetBlendState(eBlendStates::Disabled);
		}

		// Indirect combined pass -- END

		// Skybox pass -- BEGIN
		{
			wnd.Gfx().SetRenderTarget(eRenderTargetType::PostProcessing, wnd.Gfx().gBuffer.GetDepthStencilView());

			wnd.Gfx().SetDepthStencilState(eDepthStencilStates::ReadOnlyLessEqual);
			wnd.Gfx().SetRasterizerState(eRasterizerStates::NoCulling);

			skybox.Draw(wnd.Gfx());
		}
		// Skybox pass -- END

		// Sprite pass -- BEGIN
		{
			wnd.Gfx().gBuffer.SetAllAsResources(wnd.Gfx().pContext.Get(), PS_GBUFFER_SLOT);
			wnd.Gfx().SetBlendState(eBlendStates::Additive);

			deferredLights.BindFlashlightBuffer(wnd.Gfx());
			dustParticles.Draw(wnd.Gfx());

			wnd.Gfx().SetBlendState(eBlendStates::Disabled);
		}
		// Sprite pass -- END

		// Clear resources
		{
			wnd.Gfx().gBuffer.ClearAllAsResourcesSlots(wnd.Gfx().pContext.Get(), PS_GBUFFER_SLOT);

			ID3D11ShaderResourceView* const nullSRV[1] = {nullptr};
			wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTRUE_SLOT_INDIRECT_LIGHT_DIRECTIONAL, 1u, nullSRV);
			wnd.Gfx().pContext->PSSetShaderResources(PS_TEXTURE_SLOT_INDIRECT_LIGHT_SPOT, 1u, nullSRV);
		}

		// Post processing pass -- BEGIN
		wnd.Gfx().HandleBloomScaling(postProcessing);

		PixelConstantBuffer<PostProcessingBuffer> ppb{wnd.Gfx(), 1u};
		ppb.Update(wnd.Gfx(), ppBuffer);
		ppb.Bind(wnd.Gfx());

		postProcessing.Draw(wnd.Gfx());
		// Post processing pass -- END

		// ImGui windows
		if (showImGui)
		{
			//ImGui::ShowDemoWindow();

			if (ImGui::Begin("Post Processing"))
			{
				ImGui::ColorPicker3("Tint", &ppBuffer.tint.x);
				ImGui::DragFloat3("Blackpoint", &ppBuffer.blackpoint.x, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat("Exposure", &ppBuffer.exposure, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat("Contrast", &ppBuffer.contrast, 0.01f, 0.0f, 10.0f, "%.2f");
				ImGui::DragFloat("Saturation", &ppBuffer.saturation, 0.01f, 0.0f, 10.0f, "%.2f");
				ImGui::Text("Bloom");
				ImGui::Checkbox("Use bloom", &wnd.Gfx().useBloom);
				ImGui::SetNextItemWidth(100);
				ImGui::SliderFloat("Bloom blending", &wnd.Gfx().bb.bloomBlending, 0.0f, 1.0f);
				ImGui::SetNextItemWidth(100);
				ImGui::SliderFloat("Bloom threshold", &wnd.Gfx().bb.bloomThreshold, 0.0f, 1.0f);
			}
			ImGui::End();

			if (ImGui::Begin("Flashlight"))
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 250.0f);
				ImGui::Text("Spot light");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::ColorPicker3("Colour##SpotColour", &flashlightInner->colour.x);
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Intensity##SpotIntensity", &flashlightIntensity, 10.0f, 0.0f, 10000.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Range", &flashlightInner->range, 10.0f, 0.0f, 10000.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Inner angle", &flashlightInner->innerAngle, 0.01f, 0.0f, flashlightInner->outerAngle, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Outer angle", &flashlightInner->outerAngle, 0.01f, flashlightInner->innerAngle, 3.14f, "%.2f");
				ImGui::Text("Bleed");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Bleed intensity mul", &flashlightBleedIntensityFactor, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Bleed angle mul", &flashlightBleedAngleMultiplier, 0.1f, 0.0f, 10.0f, "%.2f");
				ImGui::NextColumn();
				ImGui::Text("Volumetric - Inner");
				ImGui::Checkbox("Use volumetric##UseVol1", (bool*)&flashlightInner->useVolumetricLight);
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragInt("Steps##VolStep1", (int*)&flashlightInner->numberOfVolumetricSteps, 1, 1, 15);
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Scattering##VolScat1", &flashlightInner->volumetricScattering, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Intensity##VolInt1", &flashlightInner->volumetricIntensity, 0.01f, 0.0f, 10.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::SliderAngle("Angle##VolAng1", &flashlightInner->volumetricAngle, 0, 90, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Range##VolRan1", &flashlightInner->volumetricRange, 0.1f, 0.0f, 100.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Fade##VolFad1", &flashlightInner->volumetricFade, 0.1f, 0.0f, 100.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Alpha##VolAlp1", &flashlightInner->volumetricAlpha, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Text("Volumetric - Outer");
				ImGui::Checkbox("Use volumetric##UseVol2", (bool*)&flashlightOuter->useVolumetricLight);
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragInt("Steps##VolStep2", (int*)&flashlightOuter->numberOfVolumetricSteps, 1, 1, 15);
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Scattering##VolScat2", &flashlightOuter->volumetricScattering, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Intensity##VolInt2", &flashlightOuter->volumetricIntensity, 0.01f, 0.0f, 10.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::SliderAngle("Angle##VolAng2", &flashlightOuter->volumetricAngle, 0, 90, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Range##VolRan2", &flashlightOuter->volumetricRange, 0.1f, 0.0f, 100.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Fade##VolFad2", &flashlightOuter->volumetricFade, 0.1f, 0.0f, 100.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Alpha##VolAlp2", &flashlightOuter->volumetricAlpha, 0.01f, 0.0f, 1.0f, "%.2f");
			}
			ImGui::End();

			if (ImGui::Begin("Shadows"))
			{
				ImGui::Text("PCF");
				ImGui::Checkbox("Use PCF", (bool*)&shadowBuffer.usePCF);
				ImGui::DragFloat("Offset scale##OffsetPCF", &shadowBuffer.offsetScalePCF, 0.0001f, 0.0f, 1.0f, "%.6f");
				ImGui::DragInt("Sample count", &shadowBuffer.sampleCountPCF, 1, 1, 25);
				ImGui::Text("Poisson");
				ImGui::Checkbox("Use Poisson##Shadow", (bool*)&shadowBuffer.usePoisson);
				ImGui::DragFloat("Offset scale##OffsetPoisson", &shadowBuffer.offsetScalePoissonDisk, 0.0001f, 0.0f, 1.0f, "%.6f");
			}
			ImGui::End();

			if (ImGui::Begin("RSM Directional"))
			{
				ImGui::Columns(2, nullptr, false);
				//ImGui::Checkbox("Use RSM##DirUse", (bool*)&rsmBufferDirectional.useDirectionalRSM);
				//ImGui::Checkbox("Only RSM##DirOnl", (bool*)&rsmBufferDirectional.onlyRSM);
				ImGui::Checkbox("Use Poisson##DirPoi", (bool*)&rsmBufferDirectional.usePoisson);
				ImGui::DragInt("Sample count##DirSam", (int*)&rsmBufferDirectional.sampleCount, 1, 1, 2000);
				ImGui::DragFloat("R Max##DirectMax", &rsmBufferDirectional.rMax, 0.001f, 0.0f, 5.0f, "%.3f");
				ImGui::DragFloat("RSM Intensity##DirInt", &rsmBufferDirectional.rsmIntensity, 10.0f, 0.0f, 100000.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::ColorPicker3("Shadow colour##DirShaCol", &rsmBufferSpot.shadowColour.x);
				ImGui::DragFloat("Shadow intensity##DirShaInt", &rsmBufferSpot.shadowColour.w, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::ColorPicker3("Ambiance colour##DirAmbCol", &rsmBufferSpot.ambianceColour.x);
				ImGui::DragFloat("Ambiance intensity##DirAmbInt", &rsmBufferSpot.ambianceColour.w, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat("Weight max##DirWeiMax", &rsmBufferDirectional.weightMax, 0.001f, 0.0f, 5.0f, "%.4f");

				ImGui::NextColumn();
				//ImGui::Checkbox("Use RSM##SpoUse", (bool*)&rsmBufferSpot.useSpotRSM);
				//ImGui::Checkbox("Only RSM##SpoOnl", (bool*)&rsmBufferSpot.onlyRSM);
				ImGui::Checkbox("Use Poisson##SpoPoi", (bool*)&rsmBufferSpot.usePoisson);
				ImGui::DragInt("Sample count##SpoSam", (int*)&rsmBufferSpot.sampleCount, 1, 1, 2000);
				ImGui::DragFloat("R Max##SpoMax", &rsmBufferSpot.rMax, 0.001f, 0.0f, 5.0f, "%.3f");
				ImGui::DragFloat("RSM Intensity##SpoInt", &rsmBufferSpot.rsmIntensity, 10.0f, 0.0f, 100000.0f, "%.2f");

				ImGui::DragInt("Combined passes", &combinedPasses, 1, 1, 4);
				ImGui::DragFloat("Weight max##SpoWeiMax", &rsmBufferSpot.weightMax, 0.001f, 0.0f, 5.0f, "%.4f");
				ImGui::DragFloat("Divide N", &rsmBufferSpot.divideN, 0.1f, 0.0f, 5.0f, "%.1f");
				ImGui::DragFloat("Divide P", &rsmBufferSpot.divideP, 0.1f, 0.0f, 5.0f, "%.1f");
				ImGui::DragInt("Final sample count", (int*)&rsmBufferSpot.sampleCountLastPass, 1, 0, 600);
				ImGui::SliderInt("Mode", (int*)&rsmBufferSpot.mode, 0, (int)eRSMMode::Count - 1, ModeEnumToString[rsmBufferDirectional.mode].c_str());
				rsmBufferDirectional.mode = rsmBufferSpot.mode;
				rsmBufferDirectional.sampleCountLastPass = rsmBufferSpot.sampleCountLastPass;
				rsmBufferDirectional.divideN = rsmBufferSpot.divideN;
				rsmBufferDirectional.divideP = rsmBufferSpot.divideP;
			}
			ImGui::End();
			//for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
			//{
			//	std::string name = "Point Light " + std::to_string(i);
			//	pointLights[i].ShowControlWindow(name.c_str());
			//}

			//for (int i = 0; i < static_cast<int>(spotLights.size()); ++i)
			//{
			//	std::string name = "Spot Light " + std::to_string(i);
			//	spotLights[i].ShowControlWindow(name.c_str());
			//}

			//sprites[0].ShowControlWindow("Sprite");

			camera.ShowControlWindow();

			// Draw all resources in GBuffer
			if (ImGui::Begin("GBuffer"))
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::Text("World Position");
				ImGui::Image(wnd.Gfx().gBuffer.GetShaderResourceViews()[0], ImVec2(512, 288));
				ImGui::Text("Albedo");
				ImGui::Image(wnd.Gfx().gBuffer.GetShaderResourceViews()[1], ImVec2(512, 288));
				ImGui::Text("Normal");
				ImGui::Image(wnd.Gfx().gBuffer.GetShaderResourceViews()[2], ImVec2(512, 288));
				ImGui::NextColumn();
				ImGui::Text("Material");
				ImGui::Image(wnd.Gfx().gBuffer.GetShaderResourceViews()[3], ImVec2(512, 288));
				ImGui::Text("Ambient Occlusion");
				ImGui::Image(wnd.Gfx().gBuffer.GetShaderResourceViews()[4], ImVec2(512, 288));
				//ImGui::Text("RSM");
				//ImGui::Image(wnd.Gfx().gBuffer.GetShaderResourceViews()[5], ImVec2(512, 288));
				ImGui::Text("Depth");
				ImGui::Image(*wnd.Gfx().gBuffer.GetDepthShaderResourceView(), ImVec2(512, 288));
			}
			ImGui::End();

			// RSM indirect lighting
			if (ImGui::Begin("RSM"))
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::Text("Directional light");
				ImGui::Image(wnd.Gfx().rsmFullscaleDirectional.pResource.Get(), ImVec2(512, 288));
				ImGui::NextColumn();
				ImGui::Text("Spot light");
				ImGui::Image(wnd.Gfx().rsmFullscaleSpot.pResource.Get(), ImVec2(512, 288));
			}
			ImGui::End();

			// Draw all resources in RSMBuffer
			if (ImGui::Begin("RSMBuffer Directional"))
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::Text("World Position");
				ImGui::Image(wnd.Gfx().directionalLightRSMBuffer.GetShaderResourceViews()[0], ImVec2(512, 288));
				ImGui::Text("Normal");
				ImGui::Image(wnd.Gfx().directionalLightRSMBuffer.GetShaderResourceViews()[1], ImVec2(512, 288));
				ImGui::NextColumn();
				ImGui::Text("Flux");
				ImGui::Image(wnd.Gfx().directionalLightRSMBuffer.GetShaderResourceViews()[2], ImVec2(512, 288));
				ImGui::Text("Depth");
				ImGui::Image(*wnd.Gfx().directionalLightRSMBuffer.GetDepthShaderResourceView(), ImVec2(512, 288));
			}
			ImGui::End();

			// Draw all resources in RSMBuffer
			if (ImGui::Begin("RSMBuffer Spotlight"))
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::Text("World Position");
				ImGui::Image(wnd.Gfx().spotLightRSMBuffer[0].GetShaderResourceViews()[0], ImVec2(512, 288));
				ImGui::Text("Normal");
				ImGui::Image(wnd.Gfx().spotLightRSMBuffer[0].GetShaderResourceViews()[1], ImVec2(512, 288));
				ImGui::NextColumn();
				ImGui::Text("Flux");
				ImGui::Image(wnd.Gfx().spotLightRSMBuffer[0].GetShaderResourceViews()[2], ImVec2(512, 288));
				ImGui::Text("Depth");
				ImGui::Image(*wnd.Gfx().spotLightRSMBuffer[0].GetDepthShaderResourceView(), ImVec2(512, 288));
			}
			ImGui::End();

			deferredLights.ShowControlWindow();
		}
		if (showStatsWindow)
		{
			ShowStatsWindow();
		}

		// End frame
		wnd.Gfx().EndFrame();
	}

	void Game::HandleInput(const float aDeltaTime)
	{
		UNREFERENCED_PARAMETER(aDeltaTime);

		while (const auto e = wnd.keyboard.ReadKey())
		{
			if (!e->IsPressed())
			{
				continue;
			}

			switch (e->GetKeyCode())
			{
			case VK_ESCAPE:
				if (wnd.CursorEnabled())
				{
					wnd.DisableCursor();
					wnd.mouse.EnableRaw();
				}
				else
				{
					wnd.EnableCursor();
					wnd.mouse.DisableRaw();
				}
				break;
			case VK_F1:
				showImGui = !showImGui;
				break;
			case VK_F2:
				showStatsWindow = !showStatsWindow;
				break;
			case VK_F3:
				drawLightDebug = !drawLightDebug;
				break;
			case 'F':
				flashlightOn = !flashlightOn;
				break;
			case 'R':
				drawRSM = !drawRSM;
				break;
			}
		}

		if (!wnd.CursorEnabled())
		{
			if (wnd.keyboard.KeyIsPressed(VK_SHIFT))
			{
				cameraSpeed = cameraSpeedBoost;
			}
			else
			{
				cameraSpeed = cameraSpeedDefault;
			}

			cameraInput = {0.0f, 0.0f, 0.0f};

			if (wnd.keyboard.KeyIsPressed('W'))
			{
				cameraInput.z += 1.0f;
			}

			if (wnd.keyboard.KeyIsPressed('A'))
			{
				cameraInput.x -= 1.0f;
			}

			if (wnd.keyboard.KeyIsPressed('S'))
			{
				cameraInput.z -= 1.0f;
			}

			if (wnd.keyboard.KeyIsPressed('D'))
			{
				cameraInput.x += 1.0f;
			}

			if (wnd.keyboard.KeyIsPressed(VK_SPACE))
			{
				cameraInput.y += 1.0f;
			}
			if (wnd.keyboard.KeyIsPressed(VK_CONTROL))
			{
				cameraInput.y -= 1.0f;
			}

			cameraVelocity = {
				Interp(cameraVelocity.x, aDeltaTime * cameraSpeed * cameraInput.x, aDeltaTime * cameraMoveInterpSpeed),
				Interp(cameraVelocity.y, aDeltaTime * cameraSpeed * cameraInput.y, aDeltaTime * cameraMoveInterpSpeed),
				Interp(cameraVelocity.z, aDeltaTime * cameraSpeed * cameraInput.z, aDeltaTime * cameraMoveInterpSpeed),
			};

			camera.Translate(cameraVelocity);
		}

		while (const auto delta = wnd.mouse.ReadRawDelta())
		{
			if (!wnd.CursorEnabled())
			{
				camera.Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
			}
		}
	}

	void Game::ShowStatsWindow()
	{
		if (ImGui::Begin("Stats"))
		{
			ImGui::Text("%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
			const std::string drawcalls = "Drawcalls:" + std::to_string(wnd.Gfx().GetDrawcallCount());
			ImGui::Text(drawcalls.c_str());
		}
		ImGui::End();
	}

	float Game::GetDistanceBetweenObjects(const DirectX::XMFLOAT3 aPosA, const DirectX::XMFLOAT3 aPosB) const
	{
		const float dx = aPosB.x - aPosA.x;
		const float dy = aPosB.y - aPosA.y;
		const float dz = aPosB.z - aPosA.z;

		const float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
		return distance;
	}
}
