#include "Game.h"
#include <Core/Utility/KakaMath.h>
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>
#include <future>
#include <random>
#include <TGAFBXImporter/source/Internal.inl>

#include "Graphics/Drawable/ModelLoader.h"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int NUM_POINT_LIGHTS = 0;
constexpr int NUM_SPOT_LIGHTS = 0;
constexpr int TERRAIN_SIZE = 1000;

constexpr int MODELS_TO_LOAD_THREADED = 10;
constexpr float SHADOW_RESOLUTION_DIVIDER = 4.0f;

namespace Kaka
{
	Game::Game()
		:
		wnd(WINDOW_WIDTH, WINDOW_HEIGHT, L"Kaka")
	{
		camera.SetPerspective(WINDOW_WIDTH, WINDOW_HEIGHT, 110, 0.5f, 5000.0f);
		directionalLightShadowCamera.SetOrthographic(WINDOW_WIDTH / 4.0f, WINDOW_HEIGHT / 4.0f, -500.0f, 500.0f);

		for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			pointLights.emplace_back(PointLight{wnd.Gfx(), 2u});
		}

		for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
		{
			spotLights.emplace_back(SpotLight{wnd.Gfx(), 3u});
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

		//ppBuffer.bloomThreshold = 0.5f;
		ppBuffer.tint = {1.0f, 1.0f, 1.0f};
		ppBuffer.blackpoint = {0.0f, 0.0f, 0.0f};
		ppBuffer.exposure = 0.0f;
		ppBuffer.contrast = 1.0f;
		ppBuffer.saturation = 1.0f;

		skybox.Init(wnd.Gfx(), "Assets\\Textures\\Skybox\\Miramar\\", "Assets\\Textures\\Skybox\\Kurt\\");

		//reflectionPSBuffer.A = 0.7f;
		//reflectionPSBuffer.k0 = {-0.9f, 0.5f};
		//reflectionPSBuffer.k1 = {-0.2f, 0.5f};

		//constexpr float reflectPlaneHeight = -8.0f;

		//terrain.Init(wnd.Gfx(), TERRAIN_SIZE);

		for (int i = 0; i < 50; ++i)
		{
			sprites.emplace_back();
			sprites.back().Init(wnd.Gfx(), 5.0f);
			sprites.back().SetPosition({i * 10.0f, 20.0f, 0.0f});
		}

		//reflectionPlane.Init(wnd.Gfx(), terrain.GetSize() / 2.0f);
		//reflectionPlane.SetPosition({terrain.GetSize() / 2.0f, reflectPlaneHeight, terrain.GetSize() / 2.0f});

		directionalLightShadowCamera.SetPosition({0.0f, 0.0f, 0.0f});
		camera.SetPosition({0.0f, 0.0f, 0.0f});
		camera.SetRotationDegrees(34.0f, 114.0f);

		//for (int i = 0; i < MODELS_TO_LOAD_THREADED / 2; ++i)
		//{
		//	constexpr float move = 60.0f;
		//	threadedModels[i].SetPosition({780, 40.0f, 600.0f - move * (float)i});
		//	threadedModels[i].SetScale(0.1f);
		//}
		//for (int i = MODELS_TO_LOAD_THREADED / 2; i < threadedModels.size(); ++i)
		//{
		//	constexpr float move = 60.0f;
		//	threadedModels[i].SetPosition({820, 40.0f, 600.0f - move * (float)(i - 5)});
		//	threadedModels[i].SetScale(0.1f);
		//}

		//std::random_device rd;
		//std::mt19937 mt(rd());
		//{
		//	std::uniform_real_distribution<float> cDist(0.0f, 1.0f);
		//	std::uniform_real_distribution<float> rDist(50.0f, 600.0f);
		//	std::uniform_real_distribution<float> sDist(0.1f, 2.0f);

		//	for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		//	{
		//		pointLightTravelRadiuses.push_back(rDist(mt));
		//		pointLightTravelSpeeds.push_back(sDist(mt));
		//		pointLightTravelAngles.push_back(PI);

		//		pointLights[i].SetColour({cDist(mt), cDist(mt), cDist(mt)});
		//		DirectX::XMFLOAT3 pos = terrain.GetRandomVertexPosition();
		//		pos.y += 20.0f;
		//		pointLights[i].SetRadius(75.0f);
		//		pointLights[i].SetFalloff(1.0f);
		//		pointLights[i].SetIntensity(500.0f);
		//	}

		//	for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
		//	{
		//		spotLightTravelRadiuses.push_back(rDist(mt));
		//		spotLightTravelSpeeds.push_back(sDist(mt));
		//		spotLightTravelAngles.push_back(PI);

		//		spotLights[i].SetColour({cDist(mt), cDist(mt), cDist(mt)});
		//		DirectX::XMFLOAT3 pos = terrain.GetRandomVertexPosition();
		//		pos.y += 20.0f;
		//		spotLights[i].SetPosition(pos);
		//		spotLights[i].SetIntensity(3000.0f);
		//	}
		//}

		//{
		//	std::uniform_real_distribution<float> hDist(10.0f, 100.0f);
		//	std::uniform_real_distribution<float> rDist(5.0f, 60.0f);
		//	std::uniform_real_distribution<float> sDist(0.1f, 2.0f);
		//	std::uniform_real_distribution<float> scDist(1.0f, 8.0f);

		//	for (int i = 0; i < sprites.size(); ++i)
		//	{
		//		spriteTravelRadiuses.push_back(rDist(mt));
		//		spriteTravelSpeeds.push_back(sDist(mt));
		//		spriteTravelAngles.push_back(PI);

		//		DirectX::XMFLOAT3 pos = terrain.GetRandomVertexPosition();
		//		pos.y += hDist(mt);
		//		sprites[i].SetPosition(pos);
		//		sprites[i].SetScale(scDist(mt));
		//	}
		//}

		//vfxModel.LoadModel(wnd.Gfx(), "Assets\\Models\\crawler\\CH_NPC_Crawler_01_22G3S_SK.fbx", Model::eShaderType::AnimPBR);
		//vfxModel.SetTexture(ModelLoader::LoadTexture(wnd.Gfx(), "Assets\\Textures\\coolest.png", 7u));
		//vfxModel.SetPixelShader(wnd.Gfx(), L"Shaders\\ScrollingTextureVFX_PS.cso");
		//vfxModel.LoadAnimation("Assets\\Models\\crawler\\CH_NPC_CrawlerIdle__22G3S_AN.fbx");
		//vfxModel.Init();
		//vfxModel.SetPosition({terrain.GetSize() / 2.0f, 40.0f, terrain.GetSize() / 2.0f});

		//vfxModelOlle.LoadModel(wnd.Gfx(), "Assets\\Models\\muzen\\MuzenSpeaker.fbx", Model::eShaderType::PBR);
		////vfxModelOlle.LoadFBXModel(wnd.Gfx(), "Assets\\Models\\rex\\sk_rex.fbx", Model::eShaderType::AnimPBR);
		//vfxModelOlle.SetTexture(ModelLoader::LoadTexture(wnd.Gfx(), "Assets\\Textures\\coolfx.jpg", 7u));
		//vfxModelOlle.SetPixelShader(wnd.Gfx(), L"Shaders\\Ripple_VFX_PS.cso");
		//vfxModelOlle.Init();
		//vfxModelOlle.SetPosition({250.0f, -10.0f, 500.0f});
		//vfxModelOlle.SetScale(150.0f);

		// Random colour between 0 and 1
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> cDist(0.0f, 1.0f);

		for (int i = 0; i < 5; ++i)
		{
			DirectX::XMFLOAT3 colour = {cDist(mt), cDist(mt), cDist(mt)};
			models.emplace_back();
			models.back().LoadModel(wnd.Gfx(), "Assets\\Models\\crawler\\CH_NPC_Crawler_01_22G3S_SK.fbx", Model::eShaderType::PBR);
			models.back().Init();
			models.back().SetPosition({i * 50.0f, 0.0f, 000.0f});
			models.back().SetScale(0.1f);

			//// Add point light above each model
			//PointLightData& pointLight = deferredLights.AddPointLight();
			//pointLight.position = models.back().GetPosition();
			//pointLight.position.y += 25.0f;
			//pointLight.colour = colour;
			//pointLight.intensity = 500.0f;
			//pointLight.radius = 15.0f;

			//DirectX::XMFLOAT3 colour2 = {cDist(mt), cDist(mt), cDist(mt)};
			//SpotLightData& spotLight = deferredLights.AddSpotLight();
			//spotLight.position = models.back().GetPosition();
			//spotLight.position.y -= 25.0f;
			//spotLight.direction = {0.0f, -1.0f, 0.0f};
			//spotLight.colour = colour2;
			//spotLight.intensity = 5000.0f;
			//spotLight.range = 1000.0f;
			//spotLight.innerAngle = 1.5f; // Radians
			//spotLight.outerAngle = 2.5f; // Radians
		}

		flashLightTest = &deferredLights.AddSpotLight();
		flashLightTest->position = camera.GetPosition();
		DirectX::XMStoreFloat3(&flashLightTest->direction, camera.GetForwardVector());
		flashLightTest->intensity = 500.0f;
		flashLightTest->range = 1000.0f;
		flashLightTest->innerAngle = 0.1f; // Radians
		flashLightTest->outerAngle = 0.6f; // Radians
		flashLightTest->colour = {1.0f, 0.9f, 0.6f};

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

		flashLightTest->position = camera.GetPosition();
		DirectX::XMStoreFloat3(&flashLightTest->direction, camera.GetForwardVector());
		flashLightTest->direction = {flashLightTest->direction.x * -1.0f, flashLightTest->direction.y * -1.0f, flashLightTest->direction.z * -1.0f};

		//directionalLight.SetShadowCamera(directionalLightShadowCamera.GetInverseMatrix() * directionalLightShadowCamera.GetProjection());
		//directionalLight.Bind(wnd.Gfx());
		//directionalLight.Simulate(aDeltaTime);
		commonBuffer.worldToClipMatrix = camera.GetInverseView() * camera.GetProjection();
		commonBuffer.view = camera.GetView();
		//commonBuffer.view = DirectX::XMMatrixInverse(nullptr, camera.GetInverseMatrix());
		commonBuffer.projection = camera.GetProjection();
		commonBuffer.viewInverse = camera.GetInverseView();
		commonBuffer.projectionInverse = DirectX::XMMatrixInverse(nullptr, camera.GetProjection());
		commonBuffer.cameraPosition = {camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 0.0f};
		commonBuffer.resolution = wnd.Gfx().GetCurrentResolution();
		commonBuffer.currentTime = timer.GetTotalTime();

		PixelConstantBuffer<CommonBuffer> pcb{wnd.Gfx(), 4u};
		pcb.Update(wnd.Gfx(), commonBuffer);
		pcb.Bind(wnd.Gfx());

		VertexConstantBuffer<CommonBuffer> vcb{wnd.Gfx(), 4u};
		vcb.Update(wnd.Gfx(), commonBuffer);
		vcb.Bind(wnd.Gfx());

		//PixelConstantBuffer<ReflectionWaveBuffer> rwpb{wnd.Gfx(), 10u};
		//rwpb.Update(wnd.Gfx(), reflectionPSBuffer);
		//rwpb.Bind(wnd.Gfx());

		//reflectionHeightPSBuffer.height = reflectionPlane.GetPosition().y;

		//PixelConstantBuffer<ReflectionHeightBuffer> rhpb{wnd.Gfx(), 11u};
		//rhpb.Update(wnd.Gfx(), reflectionHeightPSBuffer);
		//rhpb.Bind(wnd.Gfx());

		//reflectionVSBuffer.height = reflectionPlane.GetPosition().y;

		//VertexConstantBuffer<ReflectionHeightBuffer> rhvb{wnd.Gfx(), 9u};
		//rhvb.Update(wnd.Gfx(), reflectionVSBuffer);
		//rhvb.Bind(wnd.Gfx());

		//skyboxAngle.y += skyboxSpeed * aDeltaTime;
		//skybox.Rotate(skyboxAngle);

		// Shadow map pass -- BEGIN
		{
			wnd.Gfx().StartShadows(directionalLightShadowCamera, deferredLights.GetDirectionalLightData().lightDirection);
			deferredLights.SetShadowCamera(directionalLightShadowCamera.GetInverseView() * directionalLightShadowCamera.GetProjection());
			wnd.Gfx().SetRenderTarget(eRenderTargetType::ShadowMap);
			wnd.Gfx().SetDepthStencilState(eDepthStencilStates::Normal);
			wnd.Gfx().SetRasterizerState(eRasterizerStates::FrontfaceCulling);

			// Render everything that casts shadows
			{
				//terrain.Draw(wnd.Gfx());
				for (Model& model : models)
				{
					model.Draw(wnd.Gfx(), aDeltaTime);
				}
			}

			wnd.Gfx().ResetShadows(camera);
		}
		// Shadow map pass -- END


		// GBuffer pass -- BEGIN
		{
			wnd.Gfx().gBuffer.ClearTextures(wnd.Gfx().pContext.Get());
			wnd.Gfx().gBuffer.SetAsActiveTarget(wnd.Gfx().pContext.Get(), wnd.Gfx().gBuffer.GetDepthStencilView());

			wnd.Gfx().SetRasterizerState(eRasterizerStates::BackfaceCulling);

			for (Model& model : models)
			{
				model.Draw(wnd.Gfx(), aDeltaTime);
			}

			wnd.Gfx().SetRenderTarget(eRenderTargetType::PostProcessing, nullptr);
			wnd.Gfx().gBuffer.SetAllAsResources(wnd.Gfx().pContext.Get(), 0u);

			// Lighting pass
			wnd.Gfx().BindShadows();
			deferredLights.Draw(wnd.Gfx());
			wnd.Gfx().UnbindShadows();

			wnd.Gfx().gBuffer.ClearAllAsResourcesSlots(wnd.Gfx().pContext.Get(), 0u);

			// Skybox pass
			wnd.Gfx().SetRenderTarget(eRenderTargetType::PostProcessing, wnd.Gfx().gBuffer.GetDepthStencilView());

			wnd.Gfx().SetDepthStencilState(eDepthStencilStates::ReadOnlyLessEqual);
			wnd.Gfx().SetRasterizerState(eRasterizerStates::NoCulling);

			skybox.Draw(wnd.Gfx());
		}
		// GBuffer pass -- END

		// ImGui windows
		if (showImGui)
		{
			ImGui::ShowDemoWindow();

			if (ImGui::Begin("Reflection"))
			{
				ImGui::DragFloat2("k0", &reflectionPSBuffer.k0.x, 0.01f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat2("k1", &reflectionPSBuffer.k1.x, 0.01f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat("A", &reflectionPSBuffer.A, 0.01f, -100.0f, 100.0f, "%.1f");
			}
			ImGui::End();

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
				ImGui::Text("Depth");
				ImGui::Image(wnd.Gfx().gBuffer.GetShaderResourceViews()[4], ImVec2(512, 288));
				// Show shadow map in new imgui viewport
				ImGui::Text("Shadow map");
				ImGui::Image(wnd.Gfx().shadowMap.pResource.Get(), ImVec2(512, 288));
			}
			ImGui::End();

			deferredLights.ShowControlWindow();
		}
		if (showStatsWindow)
		{
			ShowStatsWindow();
		}

		wnd.Gfx().HandleBloomScaling(postProcessing);

		PixelConstantBuffer<PostProcessingBuffer> ppb{wnd.Gfx(), 1u};
		ppb.Update(wnd.Gfx(), ppBuffer);
		ppb.Bind(wnd.Gfx());

		postProcessing.Draw(wnd.Gfx());

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

			if (wnd.keyboard.KeyIsPressed('W'))
			{
				camera.Translate({0.0f, 0.0f, aDeltaTime * cameraSpeed});
			}
			if (wnd.keyboard.KeyIsPressed('A'))
			{
				camera.Translate({-aDeltaTime * cameraSpeed, 0.0f, 0.0f});
			}
			if (wnd.keyboard.KeyIsPressed('S'))
			{
				camera.Translate({0.0f, 0.0f, -aDeltaTime * cameraSpeed});
			}
			if (wnd.keyboard.KeyIsPressed('D'))
			{
				camera.Translate({aDeltaTime * cameraSpeed, 0.0f, 0.0f});
			}
			if (wnd.keyboard.KeyIsPressed(VK_SPACE))
			{
				camera.Translate({0.0f, aDeltaTime * cameraSpeed, 0.0f});
			}
			if (wnd.keyboard.KeyIsPressed(VK_CONTROL))
			{
				camera.Translate({0.0f, -aDeltaTime * cameraSpeed, 0.0f});
			}
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
