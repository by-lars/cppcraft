#pragma once

#include "Core/Service.h"
#include <glm/glm.hpp>

namespace ZuneCraft {
	class Input : public IService {
	public:
		Input();
		~Input();

		double GetTime();

		glm::vec2 GetMovementAxies();
		glm::vec2 GetRotationAxies();

		void CheckToggleFocus();
		bool GetActionDebugWireframe();
		bool GetActionDebugGizmos();

		bool GetActionForward();
		bool GetActionBackward();
		bool GetActionLeft();
		bool GetActionRight();
		bool GetActionJump();
	
	private:
		uint64_t m_TimerFrequency;
	};
}