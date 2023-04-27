#pragma once
#include <string>
#include <iostream>

namespace Kaka
{
	struct Mesh;

	class MeshLoader
	{
	public:
		static bool LoadMesh(const std::string& aFilePath, Mesh& aOutMesh);
	};
}
