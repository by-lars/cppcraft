#include "Core/Game.h"
#include "Core/Base.h"
#include "Core/Logger.h"
#include "Graphics/GL.h"
#include "Core/Input.h"
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
		Renderer::Init();
		Input::Init();

		m_IsFlipped = 0;
		m_Frame = 0;

		m_World = new World();

		ZC_LOG("The cake is a lie");
	}

	Game::~Game() {
		delete m_Window;
		delete m_World;
		Renderer::Shutdown();
		Input::Shutdown();
	}

	void Game::Update() {
		static bool isNewTouch = false;
		static bool lastIsFlipped = m_IsFlipped;
		m_Frame++;

		//ZDKInput_GetState(&m_Input);

		//if(m_Input.TouchState.Count < 1) {
		//	isNewTouch = true;
		//}

		//if(m_Input.TouchState.Count > 0) {
		//	m_Camera.Rotate(m_Input.TouchState.Locations[0].Y, m_Input.TouchState.Locations[0].X, isNewTouch);
		//	isNewTouch = false;
		//}

		//lastIsFlipped = m_IsFlipped;
		//m_IsFlipped = m_Input.AccelerometerState.X > 60 ? true : false;
		//if(m_IsFlipped != lastIsFlipped) {
		//	m_Renderer.SetFlip(m_IsFlipped);
		//}

		glm::vec2 input = Input::GetAxies();
		m_Camera.Rotate(input, false);

		bool didPlayerMove = false;

		if(Input::GetActionForward()) {
			m_Camera.Move(Direction::FORWARD);
			didPlayerMove = true;
		}

		if (Input::GetActionBackward()) {
			m_Camera.Move(Direction::BACKWARD);
			didPlayerMove = true;
		}

		if (Input::GetActionLeft()) {
			m_Camera.Move(Direction::LEFT);
			didPlayerMove = true;
		}		
		
		if (Input::GetActionRight()) {
			m_Camera.Move(Direction::RIGHT);
			didPlayerMove = true;
		}

		Input::CheckToggleFocus();

		Renderer::SetView(m_Camera.GetViewMatrix());
		Renderer::BeginFrame();
		m_World->SetPlayerPos(m_Camera.GetPosition(), m_Camera.GetForwardVector());
		Renderer::EndFrame();

		m_Window->Update();
	}

	GameConfig& Game::GetConfig() {
		return m_GameConfig;
	}
}