#include "Graphics/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Base.h"

namespace ZuneCraft {
    Camera::Camera() {
        m_Pos = glm::vec3(0);
        m_WorldUp = glm::vec3(0, 1, 0);
        m_Pitch = 0;
        m_Yaw = 0;
        UpdateVectors();
    }

    const glm::vec3& Camera::GetForwardVector() {
        return m_Front;
    }

    const glm::vec3& Camera::GetRightVector() {
        return m_Right;
    }

    void Camera::SetPosition(const glm::vec3& pos) {
        m_Pos = pos;
    }

    void Camera::Rotate(const glm::vec2& offset, bool resetStartPoint)  {
        m_Yaw += offset.x * 0.6f;
        m_Pitch += offset.y * 0.6f;

		if (m_Pitch > 88.0f)
			m_Pitch = 88.0f;
		if (m_Pitch < -88.0f)
			m_Pitch = -88.0f;
        UpdateVectors();
    }

    glm::mat4 Camera::GetViewMatrix() {
        return m_Viewmatrix;
    }

    const glm::quat& Camera::GetOrientation() {
        return m_Orientation;
    }

    void Camera::UpdateVectors() {
        
        float yaw = glm::radians(m_Yaw);
        float pitch = glm::radians(m_Pitch);

        glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1,0,0));
        glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));

        m_Orientation = qPitch * qYaw;
        m_Orientation = glm::normalize(m_Orientation);

        glm::mat4 rotate = glm::mat4_cast(m_Orientation);
        glm::mat4 translation = glm::mat4(1.0f);
        translation = glm::translate(translation, -m_Pos);
        
        m_Viewmatrix = rotate * translation;

        m_Front.x = cos(pitch) * cos(yaw);
        m_Front.y = sin(pitch);
        m_Front.z = cos(pitch) * sin(yaw);
        m_Front = glm::normalize(m_Front);

        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }
}