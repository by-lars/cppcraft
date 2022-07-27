#pragma once
#include "Base.h"
#include "Graphics/Renderer.h"
#include "World/World.h"
#include "Graphics/Camera.h"
#include "Core/GLWindow.h"

#include <vector>

namespace ZuneCraft {
	class Game {
	public:
		Game();
		~Game();

		void Update();

		Window& GetWindow()  { return *m_Window; }
		static Game& Get() { return *s_Instance; }

	private:
		//ZDK_INPUT_STATE m_Input;
		Camera m_Camera;
		Window* m_Window;

		//World* m_World;

		bool m_IsFlipped;
		int m_Frame;

		static Game* s_Instance;
	};
}