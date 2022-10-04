#pragma once
#include "Base.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderAPI.h"
#include "World/World.h"
#include "Core/GLWindow.h"
#include "Threading/ThreadPool.h"
#include "Core/Input.h"

#include <vector>

namespace ZuneCraft {
	struct GameConfig {
		RenderAPI::API GraphicsAPI;
		uint32_t RenderDistance;
	};

	class Game {
	public:
		Game();
		~Game();

		void Update();

		Window& GetWindow()  { return *m_Window; }
		static Game& Get() { return *s_Instance; }

		GameConfig& GetConfig();
		float GetDeltaTime();

	private:
		Window* m_Window;
		World* m_World;
		Input* m_Input;
		ThreadPool* m_ThreadPool;

		float m_DeltaTime;

		static Game* s_Instance;
		GameConfig m_GameConfig;
	};
}