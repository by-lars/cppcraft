#pragma once

#include <glm/glm.hpp>

namespace ZuneCraft {
	class Input {
	public:
		static void Init();
		static void Shutdown();
		static glm::vec2 GetAxies();

		static bool GetActionDebugWireframe();
		static bool GetActionDebugGizmos();


		static bool GetActionForward();
		static bool GetActionBackward();
		static bool GetActionLeft();
		static bool GetActionRight();
	};
}