#pragma once
#include "Core/Logger.h"
#include <cstdlib>

#ifdef ZC_PLATFORM_WIN32 
#define ZC_ASSET_PATH(x) "assets/" ## x
#elif ZC_PLATFORM_ZUNE
	#define ZC_ASSET_PATH(x) "\\gametitle\\584E07D1\\Content\\" ## x
#endif

#define ZUNE_CRAFT_VERSION "0.1.0"

#define ZC_LOG(x) { std::stringstream s; s << "[INFO] " << x; ZuneCraft::Logger::Log(s); }
#define ZC_ERROR(x) { std::stringstream s; s << "[ERROR] " << x; ZuneCraft::Logger::Error(s); }
#define ZC_FATAL_ERROR(x) { std::stringstream s; s << "FATAL ERROR: " << x; ZuneCraft::Logger::Error(s); exit(-1); }

#ifndef NDEBUG
#define ZC_DEBUG(x) { std::stringstream s; s << "[DEBUG] " << x; ZuneCraft::Logger::Log(s); }
#define ZC_DEBUG_OPENGL(x) ZuneCraft::Logger::LogOpenGLError(x)
#define ZC_ASSERT(test, msg) if((test) == false) { ZC_DEBUG("Debug test failed: " << msg); exit(-1); } 
#else
#define ZC_DEBUG(x)
#define ZC_DEBUG_OPENGL(x)
#define ZC_ASSERT(x)
#endif

typedef signed __int8     int8_t;
typedef signed __int16    int16_t;
typedef signed __int32    int32_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef signed __int64    int64_t;
typedef unsigned __int64  uint64_t;

typedef unsigned int     GLuint;
typedef			 int     GLint;
typedef unsigned int	 GLenum;

namespace ZuneCraft {
	enum Result {
		FAILURE = 0,
		SUCCESS = 1
	};
}