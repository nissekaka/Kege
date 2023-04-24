#include "Game.h"
#include <External/include/assimp/Importer.hpp>
#include <External/include/assimp/scene.h>
#include <External/include/assimp/postprocess.h>
#include <DirectXMath.h>

namespace Kaka
{
	Game::Game()
		:
		wnd(1920, 1080, L"Kaka") { }

	int Game::Go()
	{
		//if (FBXLoader::LoadMesh(R"(Assets\Models\spy\vamp.fbx)", mesh))
		//{
		//	OutputDebugStringA("\nLoaded mesh successfully!");
		//}

		{
			Assimp::Importer importer;
			const auto pModel = importer.ReadFile(R"(Assets\Models\spy\spy.fbx)",
			                                      aiProcess_Triangulate |
			                                      aiProcess_JoinIdenticalVertices |
			                                      aiProcess_FlipUVs
			);
			const auto pMesh = pModel->mMeshes[0];

			vertices.reserve(pMesh->mNumVertices);
			for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
			{
				vertices.push_back({
					{pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z},
					*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i])
				});
			}

			indices.reserve(
				static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(pMesh->mNumFaces) *
				3);
			for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
			{
				const auto& face = pMesh->mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(static_cast<const unsigned short&>(face.mIndices[0]));
				indices.push_back(static_cast<const unsigned short&>(face.mIndices[1]));
				indices.push_back(static_cast<const unsigned short&>(face.mIndices[2]));
			}
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

		wnd.Gfx().BeginFrame();

		wnd.Gfx().DrawTestMesh(vertices, indices, timer.GetTotalTime(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		//wnd.Gfx().DrawTestTriangle2D();
		wnd.Gfx().DrawTestCube3D(timer.GetTotalTime(), DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f));
		wnd.Gfx().DrawTestCube3D(-timer.GetTotalTime(), DirectX::XMFLOAT3(-3.0f, 0.0f, 0.0f));

		wnd.Gfx().EndFrame();
	}

	void Game::HandleInput(const float aDeltaTime)
	{
		UNREFERENCED_PARAMETER(aDeltaTime);
	}
}
