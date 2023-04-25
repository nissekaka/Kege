#include "Game.h"
#include <External/include/imgui/imgui.h>
#include <DirectXMath.h>

namespace Kaka
{
	Game::Game()
		:
		wnd(1920, 1080, L"Kaka") { }

	int Game::Go()
	{
		model.SetPosition({0.0f,0.0f,0.0f});

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

		model.Rotate(timer.GetTotalTime());

		// Begin frame
		wnd.Gfx().BeginFrame();

		model.Draw(wnd.Gfx());
		//wnd.Gfx().DrawTestTriangle2D();
		wnd.Gfx().DrawTestCube3D(timer.GetTotalTime(), DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f));
		wnd.Gfx().DrawTestCube3D(-timer.GetTotalTime(), DirectX::XMFLOAT3(-3.0f, 0.0f, 0.0f));

		ImGui::ShowDemoWindow();

		// End frame
		wnd.Gfx().EndFrame();
	}

	void Game::HandleInput(const float aDeltaTime)
	{
		UNREFERENCED_PARAMETER(aDeltaTime);
	}
}