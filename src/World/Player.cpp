#include "World/Player.h"
#include "Core/Service.h"
#include "Graphics/Renderer.h"
#include "Core/Input.h"
#include "Core/Game.h"
#include "World/World.h"

#include "World/AABB.h"

#undef max
#undef min

namespace ZuneCraft {
	Player::Player(World& world) : m_World(world) {
		m_Renderer = Services::Get<Renderer>();
		m_Input = Services::Get<Input>();

		m_Position = glm::vec3(0, 255, 0);
		m_MovementSpeed = 0.05f;
	}

	void Player::Tick() {
		m_Camera.Rotate(m_Input->GetRotationAxies(), false);

		glm::vec2 input = m_Input->GetMovementAxies();

		glm::quat camOrientation = glm::conjugate(m_Camera.GetOrientation());

		glm::vec3 acceleration(0.0f);
		if (input.x != 0.0f || input.y != 0.0f) {
			acceleration = camOrientation * glm::vec3(input.y, 0, -input.x);
			acceleration.y = 0;
			m_Velocity += acceleration * m_MovementSpeed;
		}

		glm::vec3 nVelocity = glm::normalize(m_Velocity);
		Material voxelMoveDir = m_World.GetVoxel(m_Position + glm::vec3(m_Velocity.x, 0, m_Velocity.z));
		if (voxelMoveDir != Material::AIR) {
			m_Velocity.x = 0;
			m_Velocity.z = 0;
		}

		Material voxelBelow = m_World.GetVoxel(m_Position - glm::vec3(0, 1, 0));
		if (voxelBelow == Material::AIR) {
			m_Velocity.y -= 0.002f;
		}
		else {
			m_Velocity.y *= 0.1f;

			if (m_Input->GetActionJump()) {
				m_Velocity.y += 0.15f;
			}
		}

		m_Velocity.x *= 0.5f;
		m_Velocity.z *= 0.5f;

		m_Position += m_Velocity;

		m_Camera.SetPosition(m_Position + glm::vec3(0, 0.9f, 0));
		m_Renderer->SetView(m_Camera.GetViewMatrix());
	}
}