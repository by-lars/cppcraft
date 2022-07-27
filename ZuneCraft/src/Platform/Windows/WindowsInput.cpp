#pragma once
#include "Core/Input.h"
#include "Core/Game.h"
#include "Core/Base.h"
#include <GLFW/glfw3.h>

namespace ZuneCraft {
	void Input::Init() {
		glfwSetInputMode((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void Input::Shutdown() {

	}

	bool Input::GetActionDebugWireframe() {
		static bool debug = false;
		static float lastPressTime = 0;
		
	
		if (glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_F1) == GLFW_PRESS) {
			float pressTime = glfwGetTime();

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
			float pressTime = glfwGetTime();

			if (pressTime - lastPressTime > 0.1f) {
				debug = !debug;
			}

			lastPressTime = pressTime;
		}

		return debug;
	}

	bool Input::GetActionForward() {
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_W) == GLFW_PRESS;
	}

	bool Input::GetActionBackward() {
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_S) == GLFW_PRESS;
	}
	
	bool Input::GetActionLeft() {
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_A) == GLFW_PRESS;
	}
	
	bool Input::GetActionRight() {
		return glfwGetKey((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), GLFW_KEY_D) == GLFW_PRESS;
	}

	glm::vec2 Input::GetAxies() {
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