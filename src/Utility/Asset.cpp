#include "Utility/Asset.h"
#include "Core/Base.h"
#include "Graphics/RenderAPI.h"
#include <fstream>

#define STBI_NO_THREAD_LOCALS
#include <stb/stb_image.h>

namespace ZuneCraft {
	std::string Asset::s_WorkingDirectory = "assets/";
	
	Result Asset::GetShaderAttribs(const std::string& name, std::vector<std::string>* _out_Lines) {
		std::string localPath = s_WorkingDirectory + "shader/" + RenderAPI::GetAPIName() + "/" + name;

		std::ifstream file(localPath.c_str());

		if (file.is_open() == false) {
			ZC_FATAL_ERROR("Could not open file: " << localPath);
			return Result::FAILURE;
		}

		std::string line;
		while (std::getline(file, line)) {
			_out_Lines->push_back(line);
		}

		return Result::SUCCESS;
	}

	std::string Asset::GetShaderSource(const std::string& name) {
		std::string localPath = s_WorkingDirectory + "shader/" + RenderAPI::GetAPIName() + "/" + name;

		std::ifstream file(localPath.c_str(), std::ios::ate);

		if (file.is_open() == false) {
			ZC_FATAL_ERROR("Could not open file: " << localPath);
		}

		size_t size = file.tellg();
		file.seekg(0);

		std::string contents(size, ' ');
		file.read(&contents[0], size);
		file.close();

		return contents;
	}

	Result Asset::GetImage(const std::string& name, Image* _out_Image) {
		std::string localPath = s_WorkingDirectory + "image" + "/" + name;

		stbi_set_flip_vertically_on_load(true);

		_out_Image->Data = stbi_load(localPath.c_str(), &_out_Image->Width, &_out_Image->Height, &_out_Image->NrChannels, 0);

		if (_out_Image->Data == nullptr) {
			ZC_FATAL_ERROR("Could not load image: " << localPath);
			return Result::FAILURE;
		}

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
}