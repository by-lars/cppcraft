#include "World/Player.h"
#include "Core/Service.h"
#include "Graphics/Renderer.h"
#include "Core/Input.h"
#include "Core/Game.h"
#include "World/World.h"

namespace ZuneCraft {
	Player::Player(World& world) : m_World(world) {
		m_Renderer = Services::Get<Renderer>();
		m_Input = Services::Get<Input>();

		m_Position = glm::vec3(0, 100, 0);
		m_MovementSpeed = 0.5f;
	}

	void Player::Tick() {
		m_Camera.Rotate(m_Input->GetRotationAxies(), false);

		glm::vec2 input = m_Input->GetMovementAxies();
		glm::vec3 forward = m_Camera.GetForwardVector();
		glm::vec3 right = m_Camera.GetRightVector();

	//	forward.y = 0;
	//	right.y = 0;

		m_Velocity += forward * input.x * m_MovementSpeed;
		m_Velocity += right * input.y * m_MovementSpeed;

		//ZC_LOG("vel: " << m_Velocity.x << " " << m_Velocity.y << " " << m_Velocity.z);

		//glm::vec3 nVelocity = glm::normalize(m_Velocity);
	/*	Material voxelMoveDir = m_World.GetVoxel(m_Position + glm::vec3(m_Velocity.x, 0, m_Velocity.z));
		if (voxelMoveDir != Material::AIR) {
			m_Velocity.x = 0;
			m_Velocity.z = 0;
		}*/

		//Material voxelBelow = m_World.GetVoxel(m_Position - glm::vec3(0, 1, 0));
		//if (voxelBelow == Material::AIR) {
		//	m_Velocity.y -= 0.01f;
		//}
		//else {
		//	m_Velocity.y *= 0.1f;
		//}

		//m_Velocity.x *= 0.1f;
		//m_Velocity.z *= 0.1f;

		m_Velocity *= 0.8f;
		m_Position += m_Velocity;

		//ZC_LOG("POS: " << m_Position.x << " " << m_Position.y << " " << m_Position.z);
		m_Camera.SetPosition(m_Position + glm::vec3(0, 1.4f, 0));
		m_Renderer->SetView(m_Camera.GetViewMatrix());
	}
}