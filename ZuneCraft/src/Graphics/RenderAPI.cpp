#include "Core/Base.h"
#include "Graphics/RenderAPI.h"

#include "Platform/OpenGL/OpenGL4API.h"
//#include "Platform/OpenGL/OpenGLES2API.h"

namespace ZuneCraft {
	RenderAPI::API RenderAPI::s_API = RenderAPI::API::OPENGL_4;

	RenderAPI* RenderAPI::Create() {
		switch (s_API) {
			case RenderAPI::API::NONE: ZC_ASSERT(false, "RenderAPI None is currently not supported."); break;
			case RenderAPI::API::OPENGL_4: return new OpenGL4API(); break;
			case RenderAPI::API::OPENGL_ES_2: ZC_ASSERT(false, "OpenGLES 2.0 is currently not supported"); break;
			default: ZC_FATAL_ERROR("Unknown RenderAPI."); break;
		}
	}

	RenderAPI::API RenderAPI::GetAPI() {
		return s_API;
	}
}