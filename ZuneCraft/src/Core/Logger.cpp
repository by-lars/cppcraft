#include "Core/Logger.h"
#include <Windows.h>
#include <string>

namespace ZuneCraft {
	std::ofstream Logger::s_File = std::ofstream();

	void Logger::Init() {
#ifdef ZC_PLATFORM_WIN32
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);

		std::string logPath = "latest.log";
#endif

#ifdef ZC_PLAFORM_ZUNE
		std::string logPath = "\\xnaa\\ZuneCraft.log";
#endif 

		s_File.open(logPath);

		if (s_File.is_open() == false) {
			std::cerr << "Error: could not open log file" << std::endl;
		}
	}

	void Logger::Shutdown() {
		s_File.flush();
		s_File.close();
	}
}