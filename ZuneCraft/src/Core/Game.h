#pragma once
#include "Base.h"
#include "Graphics/Renderer.h"
#include "World/World.h"
#include "Graphics/Camera.h"
#include "Core/GLWindow.h"
#include "Graphics/RenderAPI.h"

#include <vector>

namespace ZuneCraft {
	struct GameConfig {
		RenderAPI::API GraphicsAPI;
	};

	class Game {
	public:
		Game();
		~Game();

		void Update();

		Window& GetWindow()  { return *m_Window; }
		static Game& Get() { return *s_Instance; }

		GameConfig& GetConfig();

	private:
		Camera m_Camera;
		Window* m_Window;

		World* m_World;

		bool m_IsFlipped;
		int m_Frame;

		static Game* s_Instance;

		GameConfig m_GameConfig;
	};
}