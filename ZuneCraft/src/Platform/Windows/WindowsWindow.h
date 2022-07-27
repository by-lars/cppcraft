#pragma once
#include "Core/GLWindow.h"

#include <GLFW/glfw3.h>

namespace ZuneCraft {
	class WindowsWindow : public Window {
	public:
		WindowsWindow();
		~WindowsWindow();

		bool ShouldClose() override;
		void Update() override;
		void* GetNativeWindow() override;

	private:
		static void OnWindowResized(GLFWwindow* win, int width, int height);
		GLFWwindow* m_Window;
	};
}