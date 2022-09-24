#include "Graphics/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.h"

namespace ZuneCraft {
    Camera::Camera() {

        m_Pos = glm::vec3(16,10,16);
        m_WorldUp = glm::vec3(0, 1, 0);
        m_Pitch = 0;
        m_Yaw = 0;
        UpdateVectors();
    }

    void Camera::Move(Direction dir) {
       // ZC_LOG("X " << m_Pos.x << " Y: " << m_Pos.y << " Z:" << m_Pos.z);
        switch (dir)
        {
        case ZuneCraft::FORWARD:
            m_Pos += m_Front;
            break;
        case ZuneCraft::BACKWARD:
            m_Pos -= m_Front;
            break;
        case ZuneCraft::LEFT:
            m_Pos -= m_Right;
            break;
        case ZuneCraft::RIGHT:
            m_Pos += m_Right;
            break;
        case ZuneCraft::UP:
            m_Pos += m_Up;
            break;
        case ZuneCraft::DOWN:
            m_Pos -= m_Up;
            break;
        }
    }

    void Camera::Rotate(const glm::vec2& offset, bool resetStartPoint)  {
        //static float lastX = 0;
        //static float lastY = 0;

        //if(resetStartPoint) {
        //    lastX = x;
        //    lastY = y;
        //}

        //float xOffset = x - lastX;
        //float yOffset = y - lastY;

        m_Yaw += offset.x * 0.6;
        m_Pitch += offset.y * 0.6;

		//lastX = x;
		//lastY = y;

		if (m_Pitch > 88.0f)
			m_Pitch = 88.0f;
		if (m_Pitch < -88.0f)
			m_Pitch = -88.0f;


        UpdateVectors();
    }

    glm::mat4 Camera::GetViewMatrix() {
        return glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
    }

    const glm::vec3& Camera::GetForwardVector() {
        return m_Front;
    }

    void Camera::UpdateVectors() {
        float yaw = glm::radians(m_Yaw);
        float pitch = glm::radians(m_Pitch);

        m_Front.x = cos(pitch) * cos(yaw);
        m_Front.y = sin(pitch);
        m_Front.z = cos(pitch) * sin(yaw);
        m_Front = glm::normalize(m_Front);

        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    const glm::vec3& Camera::GetPosition() {
        return m_Pos;
    }
}