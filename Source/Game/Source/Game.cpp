#include "Game.h"
#include <Core/Utility/KakaMath.h>
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>
#include <future>
#include <random>
#include <TGAFBXImporter/source/Internal.inl>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int NUM_POINT_LIGHTS = 10;
constexpr int NUM_SPOT_LIGHTS = 10;
constexpr int TERRAIN_SIZE = 1000;

constexpr int MODELS_TO_LOAD_THREADED = 10;
constexpr float SHADOW_RESOLUTION_DIVIDER = 4.0f;

namespace Kaka
{
	Game::Game()
		:
		wnd(WINDOW_WIDTH, WINDOW_HEIGHT, L"Kaka")
	{
		camera.SetPerspective(WINDOW_WIDTH, WINDOW_HEIGHT, 90, 0.5f, 5000.0f);
		directionalLightShadowCamera.SetOrthographic(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, -500.0f, 500.0f);

		for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			pointLights.emplace_back(PointLight{wnd.Gfx(), 2u});
		}

		for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
		{
			spotLights.emplace_back(SpotLight{wnd.Gfx(), 3u});
		}

		threadedModels.resize(MODELS_TO_LOAD_THREADED);
		modelLoadingThreads.resize(threadedModels.size());
		for (int i = 0; i < modelLoadingThreads.size(); ++i)
		{
			threadHasStarted.push_back(false);
		}
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

		reflectionPSBuffer.A = 0.7f;
		reflectionPSBuffer.k0 = {-0.9f, 0.5f};
		reflectionPSBuffer.k1 = {-0.2f, 0.5f};

		constexpr float reflectPlaneHeight = -8.0f;

		terrain.Init(wnd.Gfx(), TERRAIN_SIZE);

		reflectionPlane.Init(wnd.Gfx(), terrain.GetSize() / 2.0f);
		reflectionPlane.SetPosition({terrain.GetSize() / 2.0f, reflectPlaneHeight, terrain.GetSize() / 2.0f});

		directionalLightShadowCamera.SetPosition({500.0f, 0.0f, 500.0f});
		camera.SetPosition({800.0f, 60.0f, 800.0f});
		camera.SetRotationDegrees(0.0f, -180.0f);

		for (int i = 0; i < MODELS_TO_LOAD_THREADED / 2; ++i)
		{
			constexpr float move = 60.0f;
			threadedModels[i].SetPosition({780, 40.0f, 600.0f - move * (float)i});
			threadedModels[i].SetScale(0.1f);
		}
		for (int i = MODELS_TO_LOAD_THREADED / 2; i < threadedModels.size(); ++i)
		{
			constexpr float move = 60.0f;
			threadedModels[i].SetPosition({820, 40.0f, 600.0f - move * (float)(i - 5)});
			threadedModels[i].SetScale(0.1f);
		}

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> cDist(0.0f, 1.0f);
		std::uniform_real_distribution<float> rDist(50.0f, 600.0f);
		std::uniform_real_distribution<float> sDist(0.1f, 2.0f);

		for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			pointLightTravelRadiuses.push_back(rDist(mt));
			pointLightTravelSpeeds.push_back(sDist(mt));
			pointLightTravelAngles.push_back(PI);

			pointLights[i].SetColour({cDist(mt), cDist(mt), cDist(mt)});
			DirectX::XMFLOAT3 pos = terrain.GetRandomVertexPosition();
			pos.y += 20.0f;
			pointLights[i].SetRadius(75.0f);
			pointLights[i].SetFalloff(1.0f);
			pointLights[i].SetIntensity(500.0f);
		}

		for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
		{
			spotLightTravelRadiuses.push_back(rDist(mt));
			spotLightTravelSpeeds.push_back(sDist(mt));
			spotLightTravelAngles.push_back(PI);

			spotLights[i].SetColour({cDist(mt), cDist(mt), cDist(mt)});
			DirectX::XMFLOAT3 pos = terrain.GetRandomVertexPosition();
			pos.y += 20.0f;
			spotLights[i].SetPosition(pos);
			spotLights[i].SetIntensity(3000.0f);
		}

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
		aModel.LoadFBXModel(wnd.Gfx(), aModelPath, Model::eShaderType::AnimPBR);
		aModel.Init();
		modelLoadingMutex.unlock();
	}

	void Game::Update(const float aDeltaTime)
	{
		UNREFERENCED_PARAMETER(aDeltaTime);

		for (int i = 0; i < threadedModels.size(); ++i)
		{
			const float distance = GetDistanceBetweenObjects(camera.GetPosition(), threadedModels[i].GetPosition());

			if (distance < loadRadius)
			{
				// We only want to start the thread if it hasn't already been started
				if (!threadHasStarted[i])
				{
					threadHasStarted[i] = true;
					std::string path[3] = {
						"Assets\\Models\\player\\sk_player.fbx",
						"Assets\\Models\\crawler\\CH_NPC_Crawler_01_22G3S_SK.fbx",
						"Assets\\Models\\wizard\\SM_wizard.fbx",
					};
					modelLoadingThreads[i] = std::thread(&Game::LoadModelThreaded, this, path[i % 3], std::ref(threadedModels[i]));

					// We don't have to wait for this thread, it will just do its thing
					// So we detach	it
					modelLoadingThreads[i].detach();
				}
			}
		}

		//camera.SetPosition({camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z - cameraMoveSpeed * aDeltaTime});


		// Begin frame
		wnd.Gfx().BeginFrame();
		wnd.Gfx().SetCamera(camera);

		HandleInput(aDeltaTime);

		directionalLight.SetShadowCamera(directionalLightShadowCamera.GetMatrix() * directionalLightShadowCamera.GetProjection());
		directionalLight.Bind(wnd.Gfx());
		directionalLight.Simulate(aDeltaTime);

		commonBuffer.cameraPosition = {camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 0.0f};
		commonBuffer.resolution = wnd.Gfx().GetCurrentResolution();
		commonBuffer.currentTime = timer.GetTotalTime();

		PixelConstantBuffer<CommonBuffer> cb{wnd.Gfx(), 4u};
		cb.Update(wnd.Gfx(), commonBuffer);
		cb.Bind(wnd.Gfx());

		PixelConstantBuffer<ReflectionWaveBuffer> rwpb{wnd.Gfx(), 10u};
		rwpb.Update(wnd.Gfx(), reflectionPSBuffer);
		rwpb.Bind(wnd.Gfx());

		reflectionHeightPSBuffer.height = reflectionPlane.GetPosition().y;

		PixelConstantBuffer<ReflectionHeightBuffer> rhpb{wnd.Gfx(), 11u};
		rhpb.Update(wnd.Gfx(), reflectionHeightPSBuffer);
		rhpb.Bind(wnd.Gfx());

		reflectionVSBuffer.height = reflectionPlane.GetPosition().y;

		VertexConstantBuffer<ReflectionHeightBuffer> rhvb{wnd.Gfx(), 9u};
		rhvb.Update(wnd.Gfx(), reflectionVSBuffer);
		rhvb.Bind(wnd.Gfx());

		skyboxAngle.y += skyboxSpeed * aDeltaTime;
		skybox.Rotate(skyboxAngle);

		// Draw shadows
		wnd.Gfx().StartShadows(directionalLightShadowCamera, directionalLight.GetDirection());
		wnd.Gfx().SetRenderTarget(eRenderTargetType::ShadowMap);

		// Render everything that casts shadows
		{
			terrain.Draw(wnd.Gfx());

			for (Model& model : threadedModels)
			{
				model.DrawFBXPtr(wnd.Gfx());
			}
		}

		// Reflective plane and terrain
		wnd.Gfx().SetRenderTarget(eRenderTargetType::WaterReflect);
		// Setting new render target...
		// Need to set new render target before binding the resource view for the shadow map
		wnd.Gfx().ResetShadows(camera);
		wnd.Gfx().BindShadows();

		skybox.FlipScale();

		terrain.FlipScale(reflectionPlane.GetPosition().y, false);
		terrain.SetReflectShader(wnd.Gfx(), true);

		skybox.Draw(wnd.Gfx());
		terrain.SetCullingMode(eCullingMode::Front);
		terrain.Draw(wnd.Gfx());

		skybox.FlipScale();

		terrain.SetReflectShader(wnd.Gfx(), false);
		terrain.FlipScale(reflectionPlane.GetPosition().y, true);

		// Draw regular render
		wnd.Gfx().SetRenderTarget(eRenderTargetType::PostProcessing);

		for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
		{
			pointLights[i].Bind(wnd.Gfx(), camera.GetMatrix());

			pointLightTravelAngles[i] += pointLightTravelSpeeds[i] * aDeltaTime;

			float posX = pointLightTravelRadiuses[i] * std::cos(pointLightTravelAngles[i]) + TERRAIN_SIZE / 2.0f;
			float posZ = pointLightTravelRadiuses[i] * std::sin(pointLightTravelAngles[i]) + TERRAIN_SIZE / 2.0f;

			pointLights[i].SetPosition({posX, 40.0f, posZ});

			if (pointLightTravelAngles[i] > 2 * PI)
			{
				pointLightTravelAngles[i] -= 2 * PI;
			}

			if (drawLightDebug)
			{
				pointLights[i].Draw(wnd.Gfx());
			}
		}

		for (int i = 0; i < static_cast<int>(spotLights.size()); ++i)
		{
			spotLights[i].Bind(wnd.Gfx(), camera.GetMatrix());

			spotLightTravelAngles[i] -= spotLightTravelSpeeds[i] * aDeltaTime;

			float posX = spotLightTravelRadiuses[i] * std::cos(spotLightTravelAngles[i]) + TERRAIN_SIZE / 2.0f;
			float posZ = spotLightTravelRadiuses[i] * std::sin(spotLightTravelAngles[i]) + TERRAIN_SIZE / 2.0f;

			spotLights[i].SetPosition({posX, 40.0f, posZ});

			if (spotLightTravelAngles[i] > 2 * PI)
			{
				spotLightTravelAngles[i] -= 2 * PI;
			}

			if (drawLightDebug)
			{
				spotLights[i].Draw(wnd.Gfx());
			}
		}

		{
			for (int i = 0; i < threadedModels.size(); ++i)
			{
				// Point light range
				bool nearbyPointLights[50u] = {};
				bool nearbySpotLights[50u] = {};

				for (int j = 0; j < 50u; ++j)
				{
					if (j >= pointLights.size())
					{
						nearbyPointLights[j] = false;
						nearbySpotLights[j] = false;
					}
					else
					{
						const float distance = GetDistanceBetweenObjects(threadedModels[i].GetPosition(), pointLights[j].GetPosition());
						nearbyPointLights[j] = (distance <= pointLights[j].GetRadius());
						if (j >= spotLights.size())
						{
							nearbySpotLights[j] = false;
						}
						else
						{
							nearbySpotLights[j] = (distance <= spotLights[j].GetRange());
						}
					}
				}
				threadedModels[i].SetNearbyLights(nearbyPointLights, nearbySpotLights);
			}
		}

		for (Model& model : models)
		{
			// Point light range
			bool nearbyPointLights[50u] = {};
			bool nearbySpotLights[50u] = {};

			for (int i = 0; i < 50u; ++i)
			{
				if (i >= pointLights.size())
				{
					nearbyPointLights[i] = false;
					nearbySpotLights[i] = false;
				}
				else
				{
					const float distance = GetDistanceBetweenObjects(model.GetPosition(), pointLights[i].GetPosition());
					nearbyPointLights[i] = (distance <= pointLights[i].GetRadius());
					nearbySpotLights[i] = (distance <= spotLights[i].GetRange());
				}
			}
			model.SetNearbyLights(nearbyPointLights, nearbySpotLights);
			model.Draw(wnd.Gfx());
		}

		for (Model& model : threadedModels)
		{
			model.UpdatePtr(aDeltaTime);
			model.DrawFBXPtr(wnd.Gfx());
		}

		skybox.Draw(wnd.Gfx());
		terrain.SetCullingMode(eCullingMode::Back);
		for (TerrainSubset& subset : terrain.GetTerrainSubsets())
		{
			// Point light range
			bool nearbyPointLights[50u] = {};
			bool nearbySpotLights[50u] = {};

			for (int i = 0; i < 50u; ++i)
			{
				if (i >= pointLights.size())
				{
					nearbyPointLights[i] = false;
					nearbySpotLights[i] = false;
				}
				else
				{
					const float distance = GetDistanceBetweenObjects(subset.center, pointLights[i].GetPosition());
					nearbyPointLights[i] = distance <= pointLights[i].GetRadius() * 2;
					if (i >= spotLights.size())
					{
						nearbySpotLights[i] = false;
					}
					else
					{
						nearbySpotLights[i] = distance <= spotLights[i].GetRange() * 2;
					}
				}
			}
			subset.SetNearbyLights(nearbyPointLights, nearbySpotLights);
		}
		terrain.Draw(wnd.Gfx());

		wnd.Gfx().BindWaterReflectionTexture();
		wnd.Gfx().SetAlpha();
		reflectionPlane.Draw(wnd.Gfx());
		wnd.Gfx().ResetAlpha();

		// ImGui windows
		if (showImGui)
		{
			ImGui::ShowDemoWindow();

			terrain.ShowControlWindow("Terrain");
			reflectionPlane.ShowControlWindow("Reflection Plane");
			directionalLight.ShowControlWindow("Directional Light");
			//animatedModel.ShowControlWindow("Animated Model");
			//ken.ShowControlWindow("Ken");

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

			camera.ShowControlWindow();
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
