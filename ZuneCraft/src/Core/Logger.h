#pragma once
#include "Build/PlatformDefines.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace ZuneCraft {
	ZC_ENUM LogLevel {
		LALLOC = 0,
		LDEBUG,
		LLOG,
		LWARNING,
		LERROR,
		LFATAL
	};

	class Logger {
	public:	
		static void Init();
		static void Shutdown();

		static void Log(LogLevel level, std::stringstream& stream);
	};
}