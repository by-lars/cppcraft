#include "Core/Logger.h"
#include "Core/Base.h"
#include "Graphics/GL.h"
#include <windows.h>
#include <cstdlib>
#include <zdk.h>

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

	void Logger::Log(std::stringstream& stream) {
        Log(stream.str());
    }

	void Logger::Log(const std::string& msg) {
		s_File << msg << std::endl;
		s_File.flush();
    }

	void Logger::Error(std::stringstream& stream) {
		Error(stream.str());
	}
	
	void Logger::Error(const std::string& msg) {
		s_File << msg << std::endl;
		s_File.flush();

		wchar_t wtext[1024];
		std::mbstowcs(wtext, msg.c_str(), msg.length()+1);
		ZDKSystem_ShowMessageBox(wtext, MESSAGEBOX_TYPE_OK);
	}

	void Logger::LogOpenGLError(const std::string& msg) {
		GLenum error = glGetError();
		switch(error) {
			case GL_INVALID_ENUM:					ZC_ERROR(msg << ": OpenGL Error: GL_INVALID_ENUM"); break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:	ZC_ERROR(msg << ": OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION"); break;
			case GL_INVALID_OPERATION:				ZC_ERROR(msg << ": OpenGL Error: GL_INVALID_OPERATION"); break;
			case GL_OUT_OF_MEMORY:					ZC_ERROR(msg << ": OpenGL Error: GL_OUT_OF_MEMORY"); break;
		}
	}

} 