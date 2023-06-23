#pragma once
#include "Core/GLWindow.h"

//#include <GLFW/glfw3.h>

class GLFWwindow;

namespace ZuneCraft {
	class GLFWWindow : public Window {
	public:
		GLFWWindow();
		~GLFWWindow();

		bool ShouldClose() override;
		void Update() override;
		void* GetNativeWindow() override;

	private:
		static void OnWindowResized(GLFWwindow* win, int width, int height);
		GLFWwindow* m_Window;
	};
}