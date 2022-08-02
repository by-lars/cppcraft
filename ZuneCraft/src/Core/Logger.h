#pragma once
#include "Build/PlatformDefines.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace ZuneCraft {
	ZC_ENUM LogLevel {
		ALLOC,
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

		template<class... Args>
		static void Log(LogLevel level, Args... args) {
			switch (level) {
			case LogLevel::ALLOC: std::cout << "\x1B[36m[ALLOC] "; break;
			case LogLevel::DEBUGLOG: std::cout << "\x1B[96m[DEBUG] "; break;
			case LogLevel::LOG: std::cout << "\x1B[94m[LOG] "; break;
			case LogLevel::WARNING: std::cout << "\x1B[93m[WARN] "; break;
			case LogLevel::ERROR: std::cout << "\x1B[91m[ERROR] "; break;
			case LogLevel::FATAL: std::cout << "\x1B[41m[FATAL] "; break;
			}

			(std::cout << ... << args) << "\x1B[0m" << std::endl;
			(s_File << ... << args) << std::endl;
		}

	private:
		static std::ofstream s_File;
	};
}