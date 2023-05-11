#include "Game.h"
#include <Core/Utility/KakaMath.h>
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>
#include <random>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

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
				200.0f));

		for (int i = 0; i < 16; ++i)
		{
			pointLights.push_back(PointLight{wnd.Gfx(), 2u});
		}
	}

	int Game::Go()
	{
		camera.SetPosition({0.0f, 1.0f, -3.0f});

		muzen.SetScale(0.002f);
		muzen.SetPosition({-1.0f, 0.0f, 0.0f});

		pointLights[0].SetPosition({-1.0f, 1.0f, 1.0});
		pointLights[0].SetColour({1.0f, 0.0f, 0.0f});
		pointLights[1].SetPosition({-1.0f, 1.0f, -1.0});
		pointLights[1].SetColour({0.0f, 1.0f, 0.0f});
		pointLights[2].SetPosition({1.0f, 1.0f, -1.0});
		pointLights[2].SetColour({0.0f, 1.0f, 1.0f});
		pointLights[3].SetPosition({1.0f, 1.0f, 1.0});
		pointLights[3].SetColour({1.0f, 0.0f, 1.0f});

		pointLights[4].SetPosition({-1.0f, 2.0f, 1.0});
		pointLights[4].SetColour({1.0f, 0.0f, 0.0f});
		pointLights[5].SetPosition({-1.0f, 2.0f, -1.0});
		pointLights[5].SetColour({0.0f, 1.0f, 0.0f});
		pointLights[6].SetPosition({1.0f, 2.0f, -1.0});
		pointLights[6].SetColour({0.0f, 1.0f, 1.0f});
		pointLights[7].SetPosition({1.0f, 2.0f, 1.0});
		pointLights[7].SetColour({1.0f, 0.0f, 1.0f});

		pointLights[8].SetPosition({-1.0f, 3.0f, 1.0});
		pointLights[8].SetColour({1.0f, 0.0f, 0.0f});
		pointLights[9].SetPosition({-1.0f, 3.0f, -1.0});
		pointLights[9].SetColour({0.0f, 1.0f, 0.0f});
		pointLights[10].SetPosition({1.0f, 3.0f, -1.0});
		pointLights[10].SetColour({0.0f, 1.0f, 1.0f});
		pointLights[11].SetPosition({1.0f, 3.0f, 1.0});
		pointLights[11].SetColour({1.0f, 0.0f, 1.0f});

		pointLights[12].SetPosition({-1.0f, 4.0f, 1.0});
		pointLights[12].SetColour({1.0f, 0.0f, 0.0f});
		pointLights[13].SetPosition({-1.0f, 4.0f, -1.0});
		pointLights[13].SetColour({0.0f, 1.0f, 0.0f});
		pointLights[14].SetPosition({1.0f, 4.0f, -1.0});
		pointLights[14].SetColour({0.0f, 1.0f, 1.0f});
		pointLights[15].SetPosition({1.0f, 4.0f, 1.0});
		pointLights[15].SetColour({1.0f, 0.0f, 1.0f});

		spy.SetPosition({1.0f, 0.0f, 0.0f});
		spy.SetRotation({PI / 2, 0, 0});

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
		for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
		{
			pointLights[i].Bind(wnd.Gfx(), camera.GetMatrix());
			if (drawLightDebug)
			{
				pointLights[i].Draw(wnd.Gfx());
			}
		}

		spy.SetRotation({spy.GetRotation().x, timer.GetTotalTime(), spy.GetRotation().z});
		spy.Draw(wnd.Gfx());
		muzen.SetRotation({muzen.GetRotation().x, timer.GetTotalTime(), muzen.GetRotation().z});
		muzen.Draw(wnd.Gfx());
		//wnd.Gfx().DrawTestTriangle2D();
		//wnd.Gfx().DrawTestCube3D(timer.GetTotalTime(), DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f));
		//wnd.Gfx().DrawTestCube3D(-timer.GetTotalTime(), DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));

		// ImGui windows
		if (showImGui)
		{
			ImGui::ShowDemoWindow();
			spy.ShowControlWindow("Spy");
			muzen.ShowControlWindow("Muzen");
			directionalLight.ShowControlWindow("Directional Light");

			spy.ShowControlWindow("Spy");
			muzen.ShowControlWindow("Muzen");
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
		}
		ImGui::End();
	}
}
