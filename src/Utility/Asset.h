#pragma once

#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include <string>
#include <vector>

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
	
	class Asset {
	public:
		static std::string GetShaderSource(const std::string& name);
		static Result GetShaderBinary(const std::string& name, std::vector<char>* _out_Binary);
		static Result GetImage(const std::string& name, Image* _out_Image);
		static Result GetShaderAttribs(const std::string& name, std::vector<std::string>* _out_Lines);

	private:
		static std::string s_WorkingDirectory;
	};
}