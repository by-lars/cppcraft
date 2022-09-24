#include "Core/Game.h"
#include "Core/Base.h"
#include "Core/Logger.h"
#include "Graphics/GL.h"
#include "Core/Input.h"
#include "Threading/ThreadPool.h"
#include "Core/Service.h"
#include <windows.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ZuneCraft {
	Game* Game::s_Instance = NULL;

	Game::Game() {
		s_Instance = this;
		
		m_GameConfig.GraphicsAPI = RenderAPI::API::OPENGL_4;
		m_GameConfig.RenderDistance = 4;

		m_Window = Window::Create();
		
		Services::Register<ThreadPool>();
		Services::Register<Input>();
		Services::Register<Renderer>();
		Services::Initialize();

		m_Input = Services::Get<Input>();

		m_World = new World();

		ZC_LOG("The cake is a lie");
	}

	Game::~Game() {
		delete m_Window;
		delete m_World;

		Services::Shutdown();
	}

	void Game::Update() {
		m_Input->CheckToggleFocus();

		m_World->Tick();
		m_World->Render();

		m_Window->Update();
	}

	GameConfig& Game::GetConfig() {
		return m_GameConfig;
	}
}