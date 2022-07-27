#include "Utility/File.h"
#include "Core/Base.h"
#include <fstream>

#define STBI_NO_THREAD_LOCALS
#include <stb/stb_image.h>

namespace ZuneCraft {
#ifdef ZC_PLATFORM_WIN32 
	std::string File::s_WorkingDirectory = "assets\\";
#elif ZC_PLATFORM_ZUNE
	std::string File::s_WorkingDirectory = "\\gametitle\\584E07D1\\Content\\";
#endif

	std::string File::LoadTextFile(const std::string& path) {
		std::string localPath = s_WorkingDirectory + path;
		std::ifstream file(localPath.c_str(), std::ios::ate);

		if (file.is_open() == false) {
			ZC_FATAL_ERROR("Could not open file: " << path);
		}

		size_t size = file.tellg();
		file.seekg(0);

		std::string contents(size, ' ');
		file.read(&contents[0], size);
		file.close();

		return contents;
	}

	Image File::LoadImage(const std::string& path) {
		std::string localPath = s_WorkingDirectory + path;
		return Image(localPath);
	}

	Image::Image(const std::string& path) {
		stbi_set_flip_vertically_on_load(true);
		m_Data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_NrChannels, 0);
	}

	int Image::GetWidth() {
		return m_Width;
	}

	int Image::GetHeight() {
		return m_Height;
	}

	TextureFormat Image::GetFormat() {
		if (m_NrChannels == 3) {
			return TextureFormat::RGB;
		}
		else if (m_NrChannels == 4) {
			return TextureFormat::RGBA;
		}
		else {
			ZC_FATAL_ERROR("Unknown Image Format");
		}
	}

	uint8_t* Image::GetData() {
		return m_Data;
	}

	Image::~Image() {
		stbi_image_free(m_Data);
	}
}