#include "Core/Logger.h"
#include "Core/Base.h"

#include <iostream>
#include <Windows.h>

namespace ZuneCraft {
	void Logger::Init() {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}

	void Logger::Shutdown() {

	}

	void Logger::Log(LogLevel level, std::stringstream& stream) {
		switch (level) {
		case LogLevel::LALLOC: std::cout << "\x1B[36m[ALLOC] "; break;
		case LogLevel::LDEBUG: std::cout << "\x1B[96m[DEBUG] "; break;
		case LogLevel::LLOG: std::cout << "\x1B[94m[LOG] "; break;
		case LogLevel::LWARNING: std::cout << "\x1B[93m[WARN] "; break;
		case LogLevel::LERROR: std::cout << "\x1B[91m[ERROR] "; break;
		case LogLevel::LFATAL: std::cout << "\x1B[41m[FATAL] "; break;
		}

		std::cout << stream.str() << "\x1B[0m" << std::endl;
	}
}