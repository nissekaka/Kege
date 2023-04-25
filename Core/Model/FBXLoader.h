#pragma once
#include <string>
#include <iostream>

namespace Kaka
{
	struct Mesh;

	class FBXLoader
	{
	public:
		static bool LoadMesh(const std::string& aFilePath, Mesh& aOutMesh);
	};
}