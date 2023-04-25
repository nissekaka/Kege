#pragma once
#include "Core/Model/Vertex.h"
#include <vector>

namespace Kaka
{
	struct Mesh
	{
		std::vector<Vertex> vertices = {};
		std::vector<unsigned short> indices = {};
	};
}