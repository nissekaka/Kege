#include "MeshLoader.h"
#include "Core/Model/Mesh.h"
#include "Core/Model/Vertex.h"
#include <External/include/assimp/Importer.hpp>
#include <External/include/assimp/scene.h>
#include <External/include/assimp/postprocess.h>
#include <DirectXMath.h>
#include <cassert>
#include <vector>

namespace Kaka
{
	bool MeshLoader::LoadMesh(const std::string& aFilePath, Mesh& aOutMesh)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(aFilePath,
		                                         aiProcess_Triangulate |
		                                         aiProcess_JoinIdenticalVertices |
		                                         aiProcess_FlipUVs |
		                                         aiProcess_ConvertToLeftHanded
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Failed to load file: " << importer.GetErrorString() << std::endl;
			return false;
		}

		const aiMesh* mesh = scene->mMeshes[0];

		aOutMesh.vertices.reserve(mesh->mNumVertices);
		//for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		//{
		//	aOutMesh.vertices.push_back({
		//		{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
		//		*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i])
		//	});
		//}
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			const DirectX::XMFLOAT3 position{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
			DirectX::XMFLOAT2 texCoord{0.0f, 0.0f};

			// Check if the mesh has texture coordinates
			if (mesh->HasTextureCoords(0))
			{
				// Retrieve the first set of texture coordinates
				const aiVector3D& aiTexCoord = mesh->mTextureCoords[0][i];
				texCoord.x = aiTexCoord.x;
				texCoord.y = aiTexCoord.y;
			}

			aOutMesh.vertices.push_back({position, texCoord});
		}

		aOutMesh.indices.reserve(
			static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(mesh->mNumFaces) *
			3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const auto& face = mesh->mFaces[i];
			assert(face.mNumIndices == 3);
			aOutMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[0]));
			aOutMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[1]));
			aOutMesh.indices.push_back(static_cast<const unsigned short&>(face.mIndices[2]));
		}

		return true;
	}
}
