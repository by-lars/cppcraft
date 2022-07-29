#include "Core/Game.h"
#include "Core/Base.h"
#include "Graphics/RenderAPI.h"

#ifdef ZC_PLATFORM_ZUNE
#include "Platform/OpenGL/OpenGLES2API.h"
#else
#include "Platform/OpenGL/OpenGL4API.h"
#include "Platform/OpenGL/OpenGLES2API.h"
#endif


namespace ZuneCraft {
	RenderAPI* RenderAPI::Create() {
	#ifdef ZC_PLATFORM_ZUNE
		Game::Get().GetConfig().GraphicsAPI = RenderAPI::API::OPENGL_ES_2;
		return new OpenGLES2API();
	#else 
		switch (Game::Get().GetConfig().GraphicsAPI) {
			case RenderAPI::API::NONE: ZC_ASSERT(false, "RenderAPI None is currently not supported."); break;
			case RenderAPI::API::OPENGL_4: return new OpenGL4API(); break;
			case RenderAPI::API::OPENGL_ES_2: return new OpenGLES2API(); break;
			default: ZC_FATAL_ERROR("Unknown RenderAPI."); break;
		}
	#endif
	}

	RenderAPI::API RenderAPI::GetAPI() {
		return Game::Get().GetConfig().GraphicsAPI;
	}
}