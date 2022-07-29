#pragma once

#include "Core/Base.h"
#include "Utility/StrongHandle.h"
#include "Geometry/Mesher.h"
#include "Graphics/DeviceResources.h"

#include <glm/glm.hpp>
#include <vector>


namespace ZuneCraft {
	ZC_MAKE_STRONG_HANDLE(HMesh, int, -1);

	class Renderer {
	public:
		
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void SetResolution(int width, int height);
		static void SetView(const glm::mat4& viewMat);
		static void SetFlip(bool flipped);

		static HMesh BatchSubmitMesh(PolyMesh& mesh, const glm::vec3& translation);
		static void BatchDrawMesh(HMesh hMesh);
		static void BatchFreeMesh(HMesh hMesh);

		static void DrawDebugCube(const glm::vec3& translation, const glm::vec3& size, const glm::vec3& color);
	};
}