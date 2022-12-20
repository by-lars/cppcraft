#pragma once
#include "Core/Input.h"
#include "Core/Game.h"
#include "Core/Base.h"
#include <GLFW/glfw3.h>

namespace ZuneCraft {
	static bool IsFocused = true;

	Input::Input() {
		glfwSetInputMode((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_TimerFrequency);
	}

	Input::~Input() {

	}

	float Input::GetTime() {
		uint64_t time = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&time);
		return (float)time / (float)m_TimerFrequency;
	}
	
	void Input::CheckToggleFocus() {
		static float lastPressTime = 0;

		if (glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			float pressTime = (float)glfwGetTime();

			if (pressTime - lastPressTime > 0.1f) {
				IsFocused = !IsFocused;
			}

			if (IsFocused) {
				glfwSetInputMode((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			lastPressTime = pressTime;
		}
	}

	bool Input::GetActionDebugWireframe() {
		static bool debug = false;
		static float lastPressTime = 0;
		
	
		if (glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_F1) == GLFW_PRESS) {
			float pressTime = (float)glfwGetTime();

			if (pressTime - lastPressTime > 0.1f) {
				debug = !debug;
			}

			lastPressTime = pressTime;
		}
	
		return debug;
	}

	bool Input::GetActionDebugGizmos() {
		static bool debug = true;
		static float lastPressTime = 0;

		if (glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_F2) == GLFW_PRESS) {
			float pressTime = (float)glfwGetTime();

			if (pressTime - lastPressTime > 0.1f) {
				debug = !debug;
			}

			lastPressTime = pressTime;
		}

		return debug;
	}

	bool Input::GetActionForward() {
		if (!IsFocused) return false;
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_W) == GLFW_PRESS;
	}

	bool Input::GetActionBackward() {
		if (!IsFocused) return false;
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_S) == GLFW_PRESS;
	}
	
	bool Input::GetActionLeft() {
		if (!IsFocused) return false;
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_A) == GLFW_PRESS;
	}
	
	bool Input::GetActionRight() {
		if (!IsFocused) return false;
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_D) == GLFW_PRESS;
	}

	glm::vec2 Input::GetMovementAxies() {
		bool forward = glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_W) == GLFW_PRESS;
		bool backward = glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_S) == GLFW_PRESS;
		bool left = glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_A) == GLFW_PRESS;
		bool right = glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_D) == GLFW_PRESS;

		return glm::vec2(forward - backward, right - left);
	}

	glm::vec2 Input::GetRotationAxies() {
		if (!IsFocused) return glm::vec2(0,0);

		static double lastX = 0;
		static double lastY = 0;
		static bool isFirstMouse = true;

		double x, y;
		glfwGetCursorPos((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), &x, &y);

		if (isFirstMouse) {
			lastX = x;
			lastY = y;
			isFirstMouse = false;
		}

		glm::vec2 offsets((x - lastX) * 0.5f, (lastY - y) * 0.5f);

		lastX = x;
		lastY = y;

		return offsets;
	}
}