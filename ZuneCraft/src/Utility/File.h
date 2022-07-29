#pragma once

#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include <string>

namespace ZuneCraft {
	struct Image {
		//Image(Image&& other); Not supported by Zune (c++03) :(
		Image();
		~Image();

		TextureFormat GetFormat() const;

		int Width;
		int Height;
		int NrChannels;
		uint8_t* Data;
	};
	
	struct Binary {
		//Binary(Binary&& other); Not supported by Zune (c++03) :(
		Binary();
		~Binary();

		char* Data;
		size_t Size;
	};

	class File {
	public:
		static std::string LoadTextFile(const std::string& path);
		static Result LoadImageFile(const std::string& path, Image* _out_Image);
		static Result LoadBinaryFile(const std::string& path, Binary* _out_Binary);

	private:
		static std::string s_WorkingDirectory;
	};
}