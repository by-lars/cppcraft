#pragma once
#include <fstream>
#include <string>
#include <sstream>

namespace ZuneCraft {
	class Logger {
	public:
		static void Init();
		static void Shutdown();

		static void Error(std::stringstream& stream);
		static void Error(const std::string& msg);

		static void Log(std::stringstream& stream);
		static void Log(const std::string& msg);

		static void LogOpenGLError(const std::string& msg);
	};
}