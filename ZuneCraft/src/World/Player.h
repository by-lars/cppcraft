#pragma once

#include "World/Entity.h"
#include "Graphics/Camera.h"

namespace ZuneCraft {
	class Renderer;
	class Input;

	class Player : public Entity {
	public:
		Player();
		void Tick() override;

		glm::vec3 GetPos() override;

	private:
		Renderer* m_Renderer;
		Input* m_Input;
		Camera m_Camera;
	};
}