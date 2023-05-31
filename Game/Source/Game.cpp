#include "Game.h"
#include <Core/Utility/KakaMath.h>
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>
#include <random>

constexpr int WINDOW_WIDTH = 2560;
constexpr int WINDOW_HEIGHT = 1440;

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

		for (int i = 0; i < 4; ++i)
		{
			pointLights.push_back(PointLight{wnd.Gfx(),2u});
		}
	}

	int Game::Go()
	{
		skybox.Init(wnd.Gfx(), "Assets\\Textures\\Skybox\\Miramar\\", "Assets\\Textures\\Skybox\\Kurt\\");
		spy.LoadModel(wnd.Gfx(), "Assets\\Models\\spy\\spy.fbx", Model::eShaderType::Phong);
		spy.SetPosition({228.4f,69.28f,-84.0});
		spy.SetRotation({PI / 2,PI * 2 / 3,0.0f});
		//muzen.LoadModel(wnd.Gfx(), "Assets\\Models\\muzen\\MuzenSpeaker.fbx", Model::eShaderType::Phong);
		//vamp.LoadModel(wnd.Gfx(), "Assets\\Models\\vamp\\vamp.fbx", Model::eShaderType::AnimPhong);
		//vamp.SetPosition({230.0f,70.0f,-81.0f});
		//vamp.SetRotation({PI / 2,PI * 2 / 3,0.0f});

		//cube.LoadModel(wnd.Gfx(), "Assets\\Models\\cube\\animcube.fbx", Model::eShaderType::AnimPhong);
		//cube.SetPosition({230.0f,71.0f,-82.0f});

		cubeTwoBones.LoadModel(wnd.Gfx(), "Assets\\Models\\cube\\animcube_twobones.fbx", Model::eShaderType::AnimPhong);
		cubeTwoBones.SetPosition({230.0f,71.0f,-81.0f});

		camera.SetPosition({232.0f,71.0f,-83.0f});

		constexpr int terrainSize = 500;
		terrain.Init(wnd.Gfx(), terrainSize);
		terrain.SetPosition(DirectX::XMFLOAT3(-terrainSize / 2.0f, -25.0f, -terrainSize / 2.0f));

		//muzen.SetScale(0.002f);
		//muzen.SetPosition({-1.0f,0.0f,0.0f});

		pointLights[0].SetColour({0.0f,1.0f,1.0f});
		pointLights[1].SetColour({1.0f,1.0f,0.0f});
		pointLights[2].SetColour({0.0f,1.0f,0.0f});
		pointLights[3].SetColour({1.0f,0.0f,0.0f});

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

			//constexpr float radius = 300.0f;
			//constexpr float speed = 0.8f;

			//pointLightAngles[i] += speed * aDeltaTime;

			//float posX = radius * std::cos(pointLightAngles[i]);
			//float posZ = radius * std::sin(pointLightAngles[i]);

			//pointLights[i].SetPosition({posX,15.0f,posZ});

			//if (pointLightAngles[i] > 2 * PI)
			//{
			//	pointLightAngles[i] -= 2 * PI;
			//}

			if (drawLightDebug)
			{
				pointLights[i].Draw(wnd.Gfx());
			}
		}
		skyboxAngle.y += skyboxSpeed * aDeltaTime;
		skybox.Rotate(skyboxAngle);
		skybox.Draw(wnd.Gfx());

		//spy.SetRotation({spy.GetRotation().x,timer.GetTotalTime(),spy.GetRotation().z});
		spy.Draw(wnd.Gfx());
		//muzen.SetRotation({muzen.GetRotation().x,timer.GetTotalTime(),muzen.GetRotation().z});
		//muzen.Draw(wnd.Gfx());

		//cube.Update(aDeltaTime);
		//cube.Animate();
		//cube.Draw(wnd.Gfx());
		cubeTwoBones.Update(aDeltaTime);
		cubeTwoBones.Animate();
		cubeTwoBones.Draw(wnd.Gfx());

		//vamp.Update(aDeltaTime);
		//vamp.Animate();
		//vamp.Draw(wnd.Gfx());
		terrain.Draw(wnd.Gfx());

		// ImGui windows
		if (showImGui)
		{
			ImGui::ShowDemoWindow();
			spy.ShowControlWindow("Spy");
			//muzen.ShowControlWindow("Muzen");
			//vamp.ShowControlWindow("Vamp");
			//cube.ShowControlWindow("Cube");
			cubeTwoBones.ShowControlWindow("CubeTwoBones");
			terrain.ShowControlWindow("Terrain");
			directionalLight.ShowControlWindow("Directional Light");

			for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
			{
				std::string name = "Point Light " + std::to_string(i);
				pointLights[i].ShowControlWindow(name.c_str());
			}

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
				cameraSpeed = cameraSpeedNormal;
			}

			if (wnd.keyboard.KeyIsPressed('W'))
			{
				camera.Translate({0.0f,0.0f,aDeltaTime * cameraSpeed});
			}
			if (wnd.keyboard.KeyIsPressed('A'))
			{
				camera.Translate({-aDeltaTime * cameraSpeed,0.0f,0.0f});
			}
			if (wnd.keyboard.KeyIsPressed('S'))
			{
				camera.Translate({0.0f,0.0f,-aDeltaTime * cameraSpeed});
			}
			if (wnd.keyboard.KeyIsPressed('D'))
			{
				camera.Translate({aDeltaTime * cameraSpeed,0.0f,0.0f});
			}
			if (wnd.keyboard.KeyIsPressed(VK_SPACE))
			{
				camera.Translate({0.0f,aDeltaTime * cameraSpeed,0.0f});
			}
			if (wnd.keyboard.KeyIsPressed(VK_CONTROL))
			{
				camera.Translate({0.0f,-aDeltaTime * cameraSpeed,0.0f});
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
		}
		ImGui::End();
	}
}