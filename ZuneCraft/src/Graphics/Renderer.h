#pragma once

#include "Core/Base.h"
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

		static void DrawMesh(GLuint vbo, GLuint count, const glm::vec3& translation);

		static void DrawDebugCube(const glm::vec3& translation, const glm::vec3& size, const glm::vec3& color);
	};
}