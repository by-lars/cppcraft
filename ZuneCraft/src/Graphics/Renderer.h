#pragma once

#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include "Graphics/Vertex.h"
#include <glm/glm.hpp>
#include <vector>


namespace ZuneCraft {
	class Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void SetResolution(int width, int height);
		static void SetView(const glm::mat4& viewMat);
		static void SetFlip(bool flipped);

		static Id BatchSubmitMesh(std::vector<Vertex>& mesh, const glm::vec3& translation);
		static void BatchDrawMesh(Id hMesh);
		static void BatchFreeMesh(Id hMesh);
	};
}