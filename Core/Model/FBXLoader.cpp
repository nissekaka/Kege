//#include "FBXLoader.h"
//#include <External/include/assimp/Importer.hpp>
//#include <External/include/assimp/scene.h>
//#include <External/include/assimp/postprocess.h>
//#include <cassert>
//
//namespace Kaka
//{
//	bool FBXLoader::LoadMesh(const std::string& aFilePath, FBXMesh& aOutMesh)
//	{
//		Assimp::Importer importer;
//		const aiScene* scene = importer.ReadFile(aFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);
//
//		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//		{
//			std::cerr << "Failed to load FBX file: " << importer.GetErrorString() << std::endl;
//			return false;
//		}
//
//		const aiMesh* mesh = scene->mMeshes[0];
//
//		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
//		{
//			FBXVertex vertex{};
//			vertex.position[0] = mesh->mVertices[i].x;
//			vertex.position[1] = mesh->mVertices[i].y;
//			vertex.position[2] = mesh->mVertices[i].z;
//
//			vertex.normal[0] = mesh->mNormals[i].x;
//			vertex.normal[1] = mesh->mNormals[i].y;
//			vertex.normal[2] = mesh->mNormals[i].z;
//
//			if (mesh->mTextureCoords[0])
//			{
//				vertex.texcoord[0] = mesh->mTextureCoords[0][i].x;
//				vertex.texcoord[1] = mesh->mTextureCoords[0][i].y;
//			}
//			else
//			{
//				vertex.texcoord[0] = 0.0f;
//				vertex.texcoord[1] = 0.0f;
//			}
//
//			aOutMesh.vertices.push_back(vertex);
//		}
//
//		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
//		{
//			const aiFace face = mesh->mFaces[i];
//			for (unsigned int j = 0; j < face.mNumIndices; j++)
//			{
//				aOutMesh.indices.push_back(face.mIndices[j]);
//			}
//		}
//
//		return true;
//	}
//}
