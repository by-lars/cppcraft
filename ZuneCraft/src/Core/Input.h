#pragma once

#include "Core/Service.h"
#include <glm/glm.hpp>

namespace ZuneCraft {
	class Input : public IService {
	public:
		Input();
		~Input();
		void Init() override;

		glm::vec2 GetAxies();

		void CheckToggleFocus();
		bool GetActionDebugWireframe();
		bool GetActionDebugGizmos();

		bool GetActionForward();
		bool GetActionBackward();
		bool GetActionLeft();
		bool GetActionRight();
	};
}