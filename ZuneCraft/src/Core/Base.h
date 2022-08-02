#pragma once
#include "Core/Logger.h"
#include <cstdlib>

#define ZUNE_CRAFT_VERSION "0.1.0"

#define ZC_DEBUG_ALLOC(...) ZuneCraft::Logger::Log(ZuneCraft::LogLevel::ALLOC, __VA_ARGS__)
#define ZC_LOG(...) ZuneCraft::Logger::Log(ZuneCraft::LogLevel::LOG, __VA_ARGS__)
#define ZC_WARN(...) ZuneCraft::Logger::Log(ZuneCraft::LogLevel::WARNING, __VA_ARGS__)
#define ZC_ERROR(...) ZuneCraft::Logger::Log(ZuneCraft::LogLevel::ERROR, __VA_ARGS__)

#define ZC_FATAL_ERROR(...) ZuneCraft::Logger::Log(ZuneCraft::LogLevel::FATAL, __VA_ARGS__); abort();

#ifndef NDEBUG
#define ZC_DEBUG(...) ZuneCraft::Logger::Log(ZuneCraft::LogLevel::DEBUGLOG, __VA_ARGS__)
#define ZC_ASSERT(test, msg) if((test) == false) { ZC_DEBUG("Debug test failed: ", msg); exit(-1); } 
#else
#define ZC_DEBUG(x)
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
typedef			 char	 GLchar;


namespace ZuneCraft {
	ZC_ENUM Result {
		FAILURE = 0,
		SUCCESS = 1
	};
}