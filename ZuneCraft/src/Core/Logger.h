#pragma once
#include "Build/PlatformDefines.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace ZuneCraft {
	ZC_ENUM LogLevel {
		ALLOC = 0,
		DEBUGLOG,
		LOG,
		WARNING,
		ERROR,
		FATAL
	};

	class Logger {
	public:	
		static void Init();
		static void Shutdown();

		static void Log(LogLevel level, std::stringstream& stream);
	};
}