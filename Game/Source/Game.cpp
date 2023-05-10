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
	}

	int Game::Go()
	{
		camera.SetPosition({0.0f, 1.0f, -3.0f});

		muzen.SetScale(0.002f);
		//muzen.SetPosition({-1.0f, 0.0f, 0.0f});
		muzen.SetPosition({0.0f, 0.0f, 0.0f});

		//spy.SetRotation({PI / 2, 0.0f, 0.0f});
		//spy.SetPosition({1.0f, 0.0f, 0.0f});
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> c(0, 100);

		constexpr float startX = -4.5f;
		constexpr float startY = -4.5f;

		for (int y = 0; y < 10; ++y)
		{
			for (int x = 0; x < 10; ++x)
			{
				const int index = x + 10 * y;

				pointLights[index].SetPosition({startX + x, startY + y, 1.0});
				pointLights[index].SetColour({c(mt) / 100.0f, c(mt) / 100.0f, c(mt) / 100.0f});
			}
		}

		for (auto& model : pointLightModels)
		{
			model.SetScale(0.1f);
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
		for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
		{
			pointLights[i].SetModelPosition(pointLightModels[i]);
			pointLights[i].SetModelColour(pointLightModels[i]);
			pointLights[i].Bind(wnd.Gfx(), camera.GetMatrix());
		}
		for (auto& model : pointLightModels)
		{
			model.Draw(wnd.Gfx());
		}


		//spy.SetRotation({spy.GetRotation().x, timer.GetTotalTime(), spy.GetRotation().z});
		//spy.Draw(wnd.Gfx());
		muzen.SetRotation({muzen.GetRotation().x, timer.GetTotalTime(), muzen.GetRotation().z});
		muzen.Draw(wnd.Gfx());
		//wnd.Gfx().DrawTestTriangle2D();
		//wnd.Gfx().DrawTestCube3D(timer.GetTotalTime(), DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f));
		//wnd.Gfx().DrawTestCube3D(-timer.GetTotalTime(), DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));

		// ImGui windows
		if (showDemoWindow)
		{
			ImGui::ShowDemoWindow();
		}
		//spy.ShowControlWindow("Spy");
		muzen.ShowControlWindow("Muzen");
		directionalLight.ShowControlWindow("Directional Light");

		for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
		{
			std::string name = "Point Light " + std::to_string(i);
			pointLights[i].ShowControlWindow(name.c_str());
		}

		camera.ShowControlWindow();

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
				showDemoWindow = true;
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
}
