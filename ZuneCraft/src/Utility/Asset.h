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

	class Asset {
	public:
		static std::string GetShaderSource(const std::string& name);
		static Result GetShaderBinary(const std::string& name, Binary* _out_Binary);
		static Result GetImage(const std::string& name, Image* _out_Image);

	private:
		static std::string s_WorkingDirectory;
	};
}