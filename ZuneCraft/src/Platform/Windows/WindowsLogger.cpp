#include "Core/Logger.h"
#include "Core/Base.h"
#include "Graphics/GL.h"

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

	void Logger::Log(std::stringstream& stream) {
		Log(stream.str());
	}

	void Logger::Log(const std::string& msg) {
		std::cout << "\x1B[94m" << msg << "\x1B[0m" << std::endl;
	}

	void Logger::Error(std::stringstream& stream) {
		Error(stream.str());
	}

	void Logger::Error(const std::string& msg) {
		std::cerr << "\x1B[91m" << msg << "\x1B[0m" << std::endl;
	}

	void Logger::LogOpenGLError(const std::string& msg) {
		GLenum error = glGetError();
		switch (error) {
		case GL_INVALID_ENUM:					ZC_ERROR(msg << ": OpenGL Error: GL_INVALID_ENUM"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:	ZC_ERROR(msg << ": OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION"); break;
		case GL_INVALID_OPERATION:				ZC_ERROR(msg << ": OpenGL Error: GL_INVALID_OPERATION"); break;
		case GL_OUT_OF_MEMORY:					ZC_ERROR(msg << ": OpenGL Error: GL_OUT_OF_MEMORY"); break;
		}
	}

}