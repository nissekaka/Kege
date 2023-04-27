#include "Game.h"
#include <Core/Utility/KakaMath.h>
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>

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
		muzen.SetPosition({-1.0f, 0.0f, 0.0f});

		spy.SetRotation({PI / 2, 0.0f, 0.0f});
		spy.SetPosition({1.0f, 0.0f, 0.0f});

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

		spy.SetRotation({spy.GetRotation().x, timer.GetTotalTime(), spy.GetRotation().z});
		spy.Draw(wnd.Gfx());
		muzen.SetRotation({0.0f, timer.GetTotalTime(), 0.0f});
		muzen.Draw(wnd.Gfx());
		//wnd.Gfx().DrawTestTriangle2D();
		//wnd.Gfx().DrawTestCube3D(timer.GetTotalTime(), DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));
		//wnd.Gfx().DrawTestCube3D(-timer.GetTotalTime(), DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));

		// ImGui windows
		if (showDemoWindow)
		{
			ImGui::ShowDemoWindow();
		}
		spy.ShowControlWindow("Spy");
		muzen.ShowControlWindow("Muzen");
		directionalLight.ShowControlWindow("Light");
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
