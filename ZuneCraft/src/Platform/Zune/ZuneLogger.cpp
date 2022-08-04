#include "Core/Logger.h"
#include "Core/Base.h"
#include "Graphics/GL.h"
#include <windows.h>
#include <cstdlib>
#include <zdk.h>
#undef ERROR

namespace ZuneCraft {
	static std::ofstream s_File;

	void Logger::Init() {
		s_File.open("\\xnaa\\ZuneCraft.log");

		if (s_File.is_open() == false) {
			ZC_ERROR("Could not open Log File. Error Code=" << GetLastError());
		}
		std::atexit(Logger::Shutdown);

	}

	void Logger::Shutdown() {
		s_File.flush();
		s_File.close();
	}

	void Logger::Log(LogLevel level, std::stringstream& stream) {
		switch (level) {
		case LogLevel::LALLOC: std::cout << "[ALLOC] "; break;
		case LogLevel::LDEBUG: std::cout << "[DEBUG] "; break;
		case LogLevel::LLOG: std::cout << "[LOG] "; break;
		case LogLevel::LWARNING: std::cout << "[WARN] "; break;
		case LogLevel::LERROR: std::cout << "[ERROR] "; break;
		case LogLevel::LFATAL: std::cout << "[FATAL] "; break;
		}

		if (level == LERROR || level == LFATAL) {
			wchar_t wtext[1024];
			std::string msg = stream.str();
			std::mbstowcs(wtext, msg.c_str(), msg.length() + 1);
			ZDKSystem_ShowMessageBox(wtext, MESSAGEBOX_TYPE_OK);
		}
		
		s_File << stream.str() << std::endl;
		s_File.flush();
	}
} 