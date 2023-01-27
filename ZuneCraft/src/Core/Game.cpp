#include "Core/Game.h"
#include "Core/Base.h"
#include "Core/Logger.h"
#include "Core/Input.h"
#include "Core/Service.h"
#include "Threading/ThreadPool.h"

namespace ZuneCraft {
	Game* Game::s_Instance = NULL;

	/*
	* TODO LIST:
	* - FIX ZUNE VERSION
	* - OpenGLAPI: Add memory management, store the create pointers and release them when the destructor is called
	* - Renderer: Improve the way batches are handled
	* - Renderer: Add Occlusion Culling
	* - GPUStorage: Add Push and Erase methods, also usage hints for TightlyPacked and LastUsed
	* - GPUStorage: Add automatic resize
	* - Cleanup Vertex Pool
	* - World: Improve Chunk Selection, or also add to ThreadPool (this is problematic because the chunks also use the threadpool)
	* - World: Add Player Collision
	* - General: Cleanup unsused #includes
	*/

	Game::Game() {
		s_Instance = this;
		m_DeltaTime = 0; 

		m_GameConfig.GraphicsAPI = RenderAPI::API::OPENGL_4;
		m_GameConfig.RenderDistance = 4;

		m_Window		= Window::Create();

		m_ThreadPool	= Services::Initialize<ThreadPool>();
		m_Input			= Services::Initialize<Input>();
						  Services::Initialize<Renderer>();

		m_World			= new World();

		ZC_LOG("The cake is a lie");
	}

	Game::~Game() {
		delete m_Window;
		delete m_World;
		Services::Shutdown();
	}

	void Game::Update() {
		//Calculate delta time
		static float lastFrameTime = 0;
		float currentFrameTime = m_Input->GetTime();
		m_DeltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		m_Input->CheckToggleFocus();

		m_ThreadPool->RunCallbacks();

		m_World->Tick();
		m_World->Render();

		m_Window->Update();
	}

	float Game::GetDeltaTime() {
		return m_DeltaTime;
	}

	GameConfig& Game::GetConfig() {
		return m_GameConfig;
	}
}