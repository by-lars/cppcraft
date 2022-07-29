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

	Result File::LoadImageFile(const std::string& path, Image* _out_Image) {
		std::string localPath = s_WorkingDirectory + path;
		stbi_set_flip_vertically_on_load(true);

		_out_Image->Data = stbi_load(localPath.c_str(), &_out_Image->Width, &_out_Image->Height, &_out_Image->NrChannels, 0);

		if (_out_Image->Data == nullptr) {
			ZC_FATAL_ERROR("Could not load image: " << path);
			return Result::FAILURE;
		}

		return Result::SUCCESS;
	}

	Result File::LoadBinaryFile(const std::string& path, Binary* _out_Binary) {
		std::string localPath = s_WorkingDirectory + path;
		std::ifstream file(localPath.c_str(), std::ios::ate | std::ios::binary);

		if (file.is_open() == false) {
			ZC_FATAL_ERROR("Could not open file: " << path);
			return Result::FAILURE;
		}

		_out_Binary->Size = file.tellg();
		file.seekg(0);

		_out_Binary->Data = new char[_out_Binary->Size];
		file.read(_out_Binary->Data, _out_Binary->Size);
		file.close();

		return Result::SUCCESS;
	}

	TextureFormat Image::GetFormat() const {
		if (NrChannels == 3) {
			return TextureFormat::RGB;
		}
		else if (NrChannels == 4) {
			return TextureFormat::RGBA;
		}
		else {
			ZC_FATAL_ERROR("Unknown Image Format");
		}
	}

	Image::Image() {
		Data = nullptr;
		Width = 0;
		Height = 0;
		NrChannels = 0;
	}

	Image::~Image() {
		stbi_image_free(Data);
	}

	Binary::Binary() {
		Data = nullptr;
		Size = 0;
	}

	Binary::~Binary() {
		delete[] Data;
	}
}