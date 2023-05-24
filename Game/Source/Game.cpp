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
				1000.0f));

		for (int i = 0; i < 4; ++i)
		{
			pointLights.push_back(PointLight{wnd.Gfx(),2u});
		}
	}

	int Game::Go()
	{
		//spy.LoadModel(wnd.Gfx(), "Assets\\Models\\spy\\spy.fbx", Model::eShaderType::Phong);
		//muzen.LoadModel(wnd.Gfx(), "Assets\\Models\\muzen\\MuzenSpeaker.fbx", Model::eShaderType::Phong);
		//vamp.LoadModel(wnd.Gfx(), "Assets\\Models\\vamp\\vamp.fbx", Model::eShaderType::AnimPhong);
		//cube.LoadModel(wnd.Gfx(), "Assets\\Models\\cube\\animcube.fbx", Model::eShaderType::AnimPhong);

		//camera.SetPosition({0.0f,1.0f,-3.0f});

		constexpr int terrainSize = 1000;
		terrain.Init(wnd.Gfx(), terrainSize);
		terrain.SetPosition(DirectX::XMFLOAT3(-terrainSize / 2.0f, -25.0f, -terrainSize / 2.0f));

		muzen.SetScale(0.002f);
		muzen.SetPosition({-1.0f,0.0f,0.0f});

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

		if (directionalLight.ShouldSimulate())
		{
			static float sunAngle = 0.0f;
			static const float rotationSpeed = 0.8f;
			static const DirectX::XMFLOAT3 lowColor = {0.4f,0.4f,0.6f};
			static const DirectX::XMFLOAT3 highColor = {1.0f,0.8f,0.6f};
			static const float colorLerpThreshold = -0.5f;

			// Update the angle based on time and speed
			sunAngle += rotationSpeed * aDeltaTime;

			// Calculate the new direction vector using trigonometry
			DirectX::XMFLOAT3 direction;
			direction.x = std::cos(sunAngle);
			direction.y = std::sin(sunAngle);
			direction.z = 0.0f; // Assuming the light is pointing straight down

			// Set the new direction
			directionalLight.SetDirection(direction);

			// Calculate the color based on the vertical position of the light
			float colorLerp = direction.y - colorLerpThreshold;
			colorLerp = std::clamp(colorLerp, 0.0f, 1.0f); // Clamp between 0 and 1

			// Interpolate between lowColor and highColor based on the colorLerp value
			DirectX::XMFLOAT3 currentColor;
			currentColor.x = lowColor.x + colorLerp * (highColor.x - lowColor.x);
			currentColor.y = lowColor.y + colorLerp * (highColor.y - lowColor.y);
			currentColor.z = lowColor.z + colorLerp * (highColor.z - lowColor.z);

			// Set the new color
			directionalLight.SetColour(currentColor);
		}

		for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
		{
			constexpr float radius = 300.0f;
			constexpr float speed = 0.8f;

			pointLights[i].Bind(wnd.Gfx(), camera.GetMatrix());
			angle[i] += speed * aDeltaTime;

			float posX = radius * std::cos(angle[i]);
			float posZ = radius * std::sin(angle[i]);

			pointLights[i].SetPosition({posX,15.0f,posZ});

			if (angle[i] > 2 * PI)
			{
				angle[i] -= 2 * PI;
			}

			if (drawLightDebug)
			{
				pointLights[i].Draw(wnd.Gfx());
			}
		}

		//spy.SetRotation({spy.GetRotation().x,timer.GetTotalTime(),spy.GetRotation().z});
		//spy.Draw(wnd.Gfx());
		//muzen.SetRotation({muzen.GetRotation().x,timer.GetTotalTime(),muzen.GetRotation().z});
		muzen.Draw(wnd.Gfx());

		//cube.Update(aDeltaTime);
		//cube.Animate();
		//cube.Draw(wnd.Gfx());
		//vamp.Update(aDeltaTime);
		//vamp.Animate();
		vamp.Draw(wnd.Gfx());
		//wnd.Gfx().DrawTestTriangle2D();
		//wnd.Gfx().DrawTestCube3D(timer.GetTotalTime(), DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f));
		//wnd.Gfx().DrawTestCube3D(-timer.GetTotalTime(), DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));
		terrain.Draw(wnd.Gfx());

		// ImGui windows
		if (showImGui)
		{
			ImGui::ShowDemoWindow();
			//spy.ShowControlWindow("Spy");
			//muzen.ShowControlWindow("Muzen");
			//vamp.ShowControlWindow("Vamp");
			//cube.ShowControlWindow("Cube");
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