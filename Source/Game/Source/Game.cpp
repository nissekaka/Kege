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
		directionalLightShadowCamera.SetOrthographic(WINDOW_WIDTH / 6.0f, WINDOW_HEIGHT / 6.0f, -100.0f, 100.0f);

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
		camera.SetPosition({-11.0f, 28.0f, 26.0f});
		camera.SetRotationDegrees(29.0f, 138.0f);

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
			models.back().SetPosition({i * 15.0f, 0.0f, 0.0f});
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

		models.emplace_back();
		models.back().LoadModel(wnd.Gfx(), "Assets\\Models\\floor\\Floor_2x2.fbx", Model::eShaderType::PBR);
		models.back().Init();
		models.back().SetPosition({75.0f, 0.0f, 25.0f});
		models.back().SetScale(0.5f);

		for (int i = 0; i < 5; ++i)
		{
			DirectX::XMFLOAT3 colour = {cDist(mt), cDist(mt), cDist(mt)};
			models.emplace_back();
			models.back().LoadModel(wnd.Gfx(), "Assets\\Models\\crawler\\CH_NPC_Crawler_01_22G3S_SK.fbx", Model::eShaderType::PBR);
			models.back().Init();
			models.back().SetPosition({i * 15.0f, 0.0f, -15.0f});
			models.back().SetScale(0.1f);
		}

		flashLightTest = &deferredLights.AddSpotLight();
		flashLightTest->position = camera.GetPosition();
		DirectX::XMStoreFloat3(&flashLightTest->direction, camera.GetForwardVector());
		flashLightTest->intensity = 0.0f;
		flashLightTest->range = 1000.0f;
		flashLightTest->innerAngle = 0.1f; // Radians
		flashLightTest->outerAngle = 0.2f; // Radians
		flashLightTest->colour = {1.0f, 0.9f, 0.6f};

		flashLightTest2 = &deferredLights.AddSpotLight();
		flashLightTest2->position = camera.GetPosition();
		DirectX::XMStoreFloat3(&flashLightTest2->direction, camera.GetForwardVector());
		flashLightTest2->intensity = flashLightTest->intensity * 0.5f;
		flashLightTest2->range = flashLightTest->range;
		flashLightTest2->innerAngle = flashLightTest->innerAngle; // Radians
		flashLightTest2->outerAngle = std::clamp(flashLightTest->outerAngle * 10.0f, flashLightTest2->outerAngle, 3.14f); // Radians
		flashLightTest2->colour = flashLightTest->colour;

		pointLightTest = &deferredLights.AddPointLight();
		pointLightTest->position = camera.GetPosition();
		pointLightTest->intensity = 5.0f;
		pointLightTest->colour = {1.0f, 1.0f, 1.0f};
		pointLightTest->radius = 50.0f;

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

	void Game::PointLightTest(const float aDeltaTime)
	{
		// World position buffer
		ID3D11Texture2D* worldPosTexture = wnd.Gfx().gBuffer.GetTexture(0u);

		// Create a staging buffer for the world position buffer
		D3D11_TEXTURE2D_DESC stagingDesc;
		ZeroMemory(&stagingDesc, sizeof(stagingDesc));
		stagingDesc.Width = wnd.Gfx().GetCurrentResolution().x;
		stagingDesc.Height = wnd.Gfx().GetCurrentResolution().y;
		stagingDesc.MipLevels = 1;
		stagingDesc.ArraySize = 1;
		stagingDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		stagingDesc.SampleDesc.Count = 1;
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.MiscFlags = 0;

		ID3D11Texture2D* stagingTexture;
		HRESULT hr = wnd.Gfx().pDevice->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);

		if (SUCCEEDED(hr))
		{
			// Copy the data from the world position buffer to the staging buffer
			wnd.Gfx().pContext->CopyResource(stagingTexture, worldPosTexture);

			// Map the staging buffer
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			hr = wnd.Gfx().pContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);

			// Read data from the mappedStagingResource

			const UINT centerX = wnd.Gfx().GetCurrentResolution().x / 2;
			const UINT centerY = wnd.Gfx().GetCurrentResolution().y / 2;

			const float* pixelData = static_cast<float*>(mappedResource.pData) + (centerY * mappedResource.RowPitch / sizeof(float)) + centerX * 4;

			DirectX::XMFLOAT3 worldPos = {pixelData[0], pixelData[1], pixelData[2]};

			// If length of worldPos is 0, then we didn't hit anything
			if (DirectX::XMVector3Length(DirectX::XMLoadFloat3(&worldPos)).m128_f32[0] > 0.0f)
			{
				// Move slightly out from the normal
				DirectX::XMFLOAT3 cameraForward;
				DirectX::XMStoreFloat3(&cameraForward, camera.GetForwardVector());
				worldPos.x -= cameraForward.x * pointLightPositionOffsetFactor;
				worldPos.y -= cameraForward.y * pointLightPositionOffsetFactor;
				worldPos.z -= cameraForward.z * pointLightPositionOffsetFactor;

				// Lerp towards the world position
				pointLightTest->position.x = Interp(pointLightTest->position.x, worldPos.x, aDeltaTime * pointLightPositionInterpSpeed);
				pointLightTest->position.y = Interp(pointLightTest->position.y, worldPos.y, aDeltaTime * pointLightPositionInterpSpeed);
				pointLightTest->position.z = Interp(pointLightTest->position.z, worldPos.z, aDeltaTime * pointLightPositionInterpSpeed);
				pointLightTest->intensity = Interp(pointLightTest->intensity, pointLightIntensity, aDeltaTime * pointLightIntensityInterpSpeed);
				pointLightTest->radius = pointLightRadius;

				{
					// Colour buffer
					ID3D11Texture2D* colourTexture = wnd.Gfx().gBuffer.GetTexture(1u);

					// Create a staging buffer for the world position buffer
					D3D11_TEXTURE2D_DESC stagingDesc;
					ZeroMemory(&stagingDesc, sizeof(stagingDesc));
					stagingDesc.Width = wnd.Gfx().GetCurrentResolution().x;
					stagingDesc.Height = wnd.Gfx().GetCurrentResolution().y;
					stagingDesc.MipLevels = 1;
					stagingDesc.ArraySize = 1;
					stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
					stagingDesc.SampleDesc.Count = 1;
					stagingDesc.Usage = D3D11_USAGE_STAGING;
					stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
					stagingDesc.MiscFlags = 0;

					ID3D11Texture2D* stagingTexture;
					HRESULT hr = wnd.Gfx().pDevice->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);

					if (SUCCEEDED(hr))
					{
						// Copy the data from the world position buffer to the staging buffer
						wnd.Gfx().pContext->CopyResource(stagingTexture, colourTexture);

						// Map the staging buffer
						D3D11_MAPPED_SUBRESOURCE mappedResource;
						hr = wnd.Gfx().pContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);

						// Read data from the mappedStagingResource

						const UINT centerX = wnd.Gfx().GetCurrentResolution().x / 2;
						const UINT centerY = wnd.Gfx().GetCurrentResolution().y / 2;

						const unsigned char* pixelData = static_cast<unsigned char*>(mappedResource.pData) + (centerY * mappedResource.RowPitch / sizeof(unsigned char)) + centerX * 4;

						unsigned char red = pixelData[0];
						unsigned char green = pixelData[1];
						unsigned char blue = pixelData[2];

						// Convert to 0-1 range
						DirectX::XMFLOAT3 colour = {(float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f};
						// If length of worldPos is 0, then we didn't hit anything
						if (DirectX::XMVector3Length(DirectX::XMLoadFloat3(&colour)).m128_f32[0] > 0.0f)
						{
							pointLightTest->colour.x = Interp(pointLightTest->colour.x, colour.x, aDeltaTime * pointLightColourInterpSpeed);
							pointLightTest->colour.y = Interp(pointLightTest->colour.y, colour.y, aDeltaTime * pointLightColourInterpSpeed);
							pointLightTest->colour.z = Interp(pointLightTest->colour.z, colour.z, aDeltaTime * pointLightColourInterpSpeed);
						}

						// Unmap the staging buffer
						wnd.Gfx().pContext->Unmap(stagingTexture, 0);

						// Release the staging buffer when done
					}
					stagingTexture->Release();
				}
			}
			else
			{
				pointLightTest->intensity = Interp(pointLightTest->intensity, 0.0f, aDeltaTime * pointLightIntensityInterpSpeed);
			}

			// Unmap the staging buffer
			wnd.Gfx().pContext->Unmap(stagingTexture, 0);

			// Release the staging buffer when done
		}
		stagingTexture->Release();
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

		// Flashlight test
		{
			flashLightTest->position = camera.GetPosition();
			DirectX::XMStoreFloat3(&flashLightTest->direction, camera.GetForwardVector());
			flashLightTest->direction = {flashLightTest->direction.x * -1.0f, flashLightTest->direction.y * -1.0f, flashLightTest->direction.z * -1.0f};

			flashLightTest2->position = camera.GetPosition();
			DirectX::XMStoreFloat3(&flashLightTest2->direction, camera.GetForwardVector());
			flashLightTest2->direction = {flashLightTest2->direction.x * -1.0f, flashLightTest2->direction.y * -1.0f, flashLightTest2->direction.z * -1.0f};
			flashLightTest2->intensity = flashLightTest->intensity * flashLightBleedIntensityFactor;
			flashLightTest2->range = flashLightTest->range;
			flashLightTest2->innerAngle = flashLightTest->innerAngle; // Radians
			flashLightTest2->outerAngle = std::clamp(flashLightTest->outerAngle * flashLightBleedAngleMultiplier, flashLightTest->outerAngle, 3.14f); // Radians
			flashLightTest2->colour = flashLightTest->colour;
		}

		commonBuffer.worldToClipMatrix = camera.GetInverseView() * camera.GetProjection();
		commonBuffer.view = camera.GetView();
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


		// Shadow map pass -- BEGIN
		{
			wnd.Gfx().StartShadows(directionalLightShadowCamera, deferredLights.GetDirectionalLightData().lightDirection);
			deferredLights.SetShadowCamera(directionalLightShadowCamera.GetInverseView() * directionalLightShadowCamera.GetProjection());
			wnd.Gfx().SetRenderTarget(eRenderTargetType::ShadowMap);
			wnd.Gfx().SetDepthStencilState(eDepthStencilStates::Normal);
			// Need backface culling for Reflective Shadow Maps
			wnd.Gfx().SetRasterizerState(eRasterizerStates::BackfaceCulling);

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

			//wnd.Gfx().SetRasterizerState(eRasterizerStates::BackfaceCulling);

			for (Model& model : models)
			{
				model.Draw(wnd.Gfx(), aDeltaTime);
			}

			wnd.Gfx().SetRenderTarget(eRenderTargetType::PostProcessing, nullptr);
			wnd.Gfx().gBuffer.SetAllAsResources(wnd.Gfx().pContext.Get(), 0u);


			PixelConstantBuffer<ShadowBuffer> shadowPixelBuffer{wnd.Gfx(), 7u};
			shadowPixelBuffer.Update(wnd.Gfx(), shadowBuffer);
			shadowPixelBuffer.Bind(wnd.Gfx());


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

		// Point light flashlight test
		{
			//PointLightTest(aDeltaTime);
		}


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
				ImGui::Text("Spoit light");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::ColorPicker3("Colour##SpotColour", &flashLightTest->colour.x);
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Intensity##SpotIntensity", &flashLightTest->intensity, 10.0f, 0.0f, 10000.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Range", &flashLightTest->range, 10.0f, 0.0f, 10000.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Inner angle", &flashLightTest->innerAngle, 0.01f, 0.0f, flashLightTest->outerAngle, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Outer angle", &flashLightTest->outerAngle, 0.01f, flashLightTest->innerAngle, 3.14f, "%.2f");
				ImGui::Text("Bleed");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Bleed intensity mul", &flashLightBleedIntensityFactor, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Bleed angle mul", &flashLightBleedAngleMultiplier, 0.1f, 0.0f, 10.0f, "%.2f");
				ImGui::NextColumn();
				ImGui::SetColumnWidth(0, 250.0f);
				ImGui::Text("Point light");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Offset factor", &pointLightPositionOffsetFactor, 0.01f, 0.0f, 10.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Position speed", &pointLightPositionInterpSpeed, 1.0f, 0.0f, 500.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Colour speed", &pointLightColourInterpSpeed, 1.0f, 0.0f, 500.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Intensity speed", &pointLightIntensityInterpSpeed, 1.0f, 0.0f, 500.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Intensity##PointIntensity", &pointLightIntensity, 1.0f, 0.0f, 10000.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::DragFloat("Radius", &pointLightRadius, 1.0f, 0.0f, 1000.0f, "%.2f");
				ImGui::SetNextItemWidth(150.0f);
				ImGui::ColorPicker3("Colour##PointColour", &pointLightTest->colour.x);
			}
			ImGui::End();

			if (ImGui::Begin("Shadows"))
			{
				ImGui::Text("PCF");
				ImGui::Checkbox("Use PCF", (bool*)&shadowBuffer.usePCF);
				ImGui::DragFloat("Offset scale##OffsetPCF", &shadowBuffer.offsetScalePCF, 0.0001f, 0.0f, 1.0f, "%.6f");
				ImGui::DragInt("Sample count", &shadowBuffer.sampleCountPCF, 1, 1, 25);
				ImGui::Text("Poisson");
				ImGui::Checkbox("Use Poisson", (bool*)&shadowBuffer.usePoisson);
				ImGui::DragFloat("Offset scale##OffsetPoisson", &shadowBuffer.offsetScalePoissonDisk, 0.0001f, 0.0f, 1.0f, "%.6f");
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
