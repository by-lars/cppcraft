#pragma once
#include "Core/Logger.h"
#include <cstdlib>
#include <sstream>
#include <memory>

#define ZUNE_CRAFT_VERSION "0.1.0"

#define ZC_DEBUG_ALLOC(msg) { std::stringstream s; s << msg; Logger::Log(LogLevel::LALLOC, s); }
#define ZC_LOG(msg) { std::stringstream s; s << msg; Logger::Log(LogLevel::LLOG, s); }
#define ZC_WARN(msg) { std::stringstream s; s << msg; Logger::Log(LogLevel::LWARNING, s); }
#define ZC_ERROR(msg) { std::stringstream s; s << msg; Logger::Log(LogLevel::LERROR, s); }

#define ZC_FATAL_ERROR(msg) { std::stringstream s; s << msg; Logger::Log(LogLevel::LFATAL, s); abort(); }

#ifndef NDEBUG
#define ZC_DEBUG(msg) { std::stringstream s; s << msg; Logger::Log(LogLevel::LDEBUG, s); }
#define ZC_ASSERT(test, msg) if((test) == false) { ZC_FATAL_ERROR(__FILE__ << "(" << __LINE__ << "): " << "Debug test failed: " << msg); abort(); } 
#else
#define ZC_DEBUG(x) 
#define ZC_ASSERT(x, y) 
#endif

typedef unsigned int     GLuint;
typedef			 int     GLint;
typedef unsigned int	 GLenum;
typedef			 char	 GLchar;
typedef			 int	 GLsizei;

#ifndef UINT32_MAX
#define UINT32_MAX  (0xffffffff)
#endif

namespace ZuneCraft {
	enum class Result {
		FAILURE = 0,
		SUCCESS = 1
	};

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

