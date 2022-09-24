#pragma once

#include <glm/glm.hpp>

namespace ZuneCraft {
	class Entity {
	public:
		virtual void SetPos(glm::vec3 pos) { m_Position = pos; };
		virtual glm::vec3 GetPos() { return m_Position; };

		virtual void Tick() = 0;
		
	private:
		glm::vec3 m_Position;
	};
}