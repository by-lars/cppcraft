#include "World/Player.h"
#include "Core/Service.h"
#include "Graphics/Renderer.h"
#include "Core/Input.h"

namespace ZuneCraft {
	Player::Player() {
		m_Renderer = Services::Get<Renderer>();
		m_Input = Services::Get<Input>();
	}

	void Player::Tick() {
		glm::vec2 input = m_Input->GetAxies();
		m_Camera.Rotate(input, false);

		bool didPlayerMove = false;

		if (m_Input->GetActionForward()) {
			m_Camera.Move(Direction::FORWARD);
			didPlayerMove = true;
		}

		if (m_Input->GetActionBackward()) {
			m_Camera.Move(Direction::BACKWARD);
			didPlayerMove = true;
		}

		if (m_Input->GetActionLeft()) {
			m_Camera.Move(Direction::LEFT);
			didPlayerMove = true;
		}

		if (m_Input->GetActionRight()) {
			m_Camera.Move(Direction::RIGHT);
			didPlayerMove = true;
		}

		m_Renderer->SetView(m_Camera.GetViewMatrix());
	}

	glm::vec3 Player::GetPos() {
		return m_Camera.GetPosition();
	}
}