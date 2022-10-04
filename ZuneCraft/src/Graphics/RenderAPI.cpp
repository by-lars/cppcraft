#include "Core/Game.h"
#include "Core/Base.h"
#include "Graphics/RenderAPI.h"

#include "Platform/OpenGL/GLAPI.h"



namespace ZuneCraft {
	RenderAPI* RenderAPI::Create() {
	#ifdef ZC_PLATFORM_ZUNE
		Game::Get().GetConfig().GraphicsAPI = RenderAPI::API::OPENGL_ES_2;
		return new OpenGLES2API();
	#else 
		switch (Game::Get().GetConfig().GraphicsAPI) {
			case RenderAPI::API::NONE: ZC_ASSERT(false, "RenderAPI None is currently not supported."); break;

			case RenderAPI::API::OPENGL_4:
			case RenderAPI::API::OPENGL_ES_2: 
				return new OpenGLAPI(); break;

			default: ZC_FATAL_ERROR("Unknown RenderAPI."); break;
		}
	#endif
	}

	RenderAPI::API RenderAPI::GetAPI() {
		return Game::Get().GetConfig().GraphicsAPI;
	}

	std::string RenderAPI::GetAPIName() {
		switch (GetAPI()){
			case RenderAPI::API::NONE: return "No API"; break;
			case RenderAPI::API::OPENGL_4: return "GL4"; break;
			case RenderAPI::API::OPENGL_ES_2: return "GLES2"; break;
			default: return "Unknown API"; break;
		}
	}

	const RenderAPI::Capabilities& RenderAPI::GetCapabilities() {
		return m_Capabilities;
	}
}