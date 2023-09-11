#include "Game.h"
#include <Core/Utility/KakaMath.h>
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>
#include <random>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int NUM_LIGHTS = 0;
constexpr int TERRAIN_SIZE = 1000;

namespace Kaka
{
	Game::Game()
		:
		wnd(WINDOW_WIDTH, WINDOW_HEIGHT, L"Kaka")
	{
		wnd.Gfx().SetProjection(
			DirectX::XMMatrixPerspectiveLH(
				1.0f,
				static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(WINDOW_WIDTH),
				0.5f,
				5000.0f));

		for (int i = 0; i < NUM_LIGHTS; ++i)
		{
			pointLights.push_back(PointLight{wnd.Gfx(), 2u});
		}
	}

	int Game::Go()
	{
		skybox.Init(wnd.Gfx(), "Assets\\Textures\\Skybox\\Miramar\\", "Assets\\Textures\\Skybox\\Kurt\\");

		reflectionPSBuffer.A = 0.7f;
		reflectionPSBuffer.k0 = {-0.9f, 0.5f};
		reflectionPSBuffer.k1 = {-0.2f, 0.5f};

		constexpr float reflectPlaneHeight = -8.0f;

		reflectionPlane.Init(wnd.Gfx(), TERRAIN_SIZE / 2.0f);
		reflectionPlane.SetPosition({TERRAIN_SIZE / 2.0f, reflectPlaneHeight, TERRAIN_SIZE / 2.0f});

		camera.SetPosition({742.75f, 0.16f, 395.95f});

		terrain.Init(wnd.Gfx(), TERRAIN_SIZE);

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> cDist(0.0f, 1.0f);
		std::uniform_real_distribution<float> rDist(50.0f, 600.0f);
		std::uniform_real_distribution<float> sDist(0.1f, 2.0f);

		for (int i = 0; i < NUM_LIGHTS; ++i)
		{
			lightTravelRadiuses.push_back(rDist(mt));
			lightTravelSpeeds.push_back(sDist(mt));
			lightTravelAngles.push_back(PI);

			pointLights[i].SetColour({cDist(mt), cDist(mt), cDist(mt)});
			DirectX::XMFLOAT3 pos = terrain.GetRandomVertexPosition();
			pos.y += 20.0f;
			pointLights[i].SetPosition(pos);
			pointLights[i].SetRadius(75.0f);
			pointLights[i].SetFalloff(1.0f);
			pointLights[i].SetIntensity(500.0f);
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

	void Game::Update(const float aDeltaTime)
	{
		UNREFERENCED_PARAMETER(aDeltaTime);

		// Begin frame
		wnd.Gfx().BeginFrame();
		wnd.Gfx().SetCamera(camera.GetMatrix());

		HandleInput(aDeltaTime);

		directionalLight.Bind(wnd.Gfx());
		directionalLight.Simulate(aDeltaTime);

		for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
		{
			pointLights[i].Bind(wnd.Gfx(), camera.GetMatrix());

			lightTravelAngles[i] += lightTravelSpeeds[i] * aDeltaTime;

			float posX = lightTravelRadiuses[i] * std::cos(lightTravelAngles[i]) + TERRAIN_SIZE / 2.0f;
			float posZ = lightTravelRadiuses[i] * std::sin(lightTravelAngles[i]) + TERRAIN_SIZE / 2.0f;

			pointLights[i].SetPosition({posX, 40.0f, posZ});

			if (lightTravelAngles[i] > 2 * PI)
			{
				lightTravelAngles[i] -= 2 * PI;
			}

			if (drawLightDebug)
			{
				pointLights[i].Draw(wnd.Gfx());
			}
		}

		commonBuffer.cameraPosition = {camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 0.0f};
		commonBuffer.resolution = wnd.Gfx().GetCurrentResolution();
		commonBuffer.currentTime = timer.GetTotalTime();

		PixelConstantBuffer<CommonBuffer> cb{wnd.Gfx(), 3u};
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

		VertexConstantBuffer<ReflectionHeightBuffer> rhvb{wnd.Gfx(), 1u};
		rhvb.Update(wnd.Gfx(), reflectionVSBuffer);
		rhvb.Bind(wnd.Gfx());

		skyboxAngle.y += skyboxSpeed * aDeltaTime;
		skybox.Rotate(skyboxAngle);

		// Reflective plane and terrain
		wnd.Gfx().SetWaterReflectTarget();

		skybox.FlipScale();
		terrain.FlipScale(reflectionPlane.GetPosition().y, false);
		terrain.SetReflectShader(wnd.Gfx(), true);

		skybox.Draw(wnd.Gfx());
		terrain.SetCullingMode(eCullingMode::Front);
		terrain.Draw(wnd.Gfx());

		skybox.FlipScale();
		terrain.SetReflectShader(wnd.Gfx(), false);
		terrain.FlipScale(reflectionPlane.GetPosition().y, true);
		wnd.Gfx().SetDefaultTarget();

		skybox.Draw(wnd.Gfx());
		terrain.SetCullingMode(eCullingMode::Back);
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

			if (ImGui::Begin("Reflection"))
			{
				ImGui::DragFloat2("k0", &reflectionPSBuffer.k0.x, 0.01f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat2("k1", &reflectionPSBuffer.k1.x, 0.01f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat("A", &reflectionPSBuffer.A, 0.01f, -100.0f, 100.0f, "%.1f");
			}
			ImGui::End();

			//for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
			//{
			//	std::string name = "Point Light " + std::to_string(i);
			//	pointLights[i].ShowControlWindow(name.c_str());
			//}

			camera.ShowControlWindow();
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
			ImGui::Text("%.3f m/s", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
			const std::string drawcalls = "Drawcalls:" + std::to_string(wnd.Gfx().GetDrawcallCount());
			ImGui::Text(drawcalls.c_str());
		}
		ImGui::End();
	}
}
