#pragma once

#include "World/Entity.h"
#include "Graphics/Camera.h"

namespace ZuneCraft {
	class Renderer;
	class Input;
	class World;

	class Player : public Entity {
	public:
		Player(World& world);
		void Tick() override;

	private:
		float m_MovementSpeed;

		World& m_World;
		Renderer* m_Renderer;
		Input* m_Input;
		Camera m_Camera;
	};
}