#pragma once

#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include <string>

namespace ZuneCraft {
	class Image {
	public:
		Image(Image&& other);
		Image(const std::string& path);
		~Image();

		int GetWidth();
		int GetHeight();
		TextureFormat GetFormat();
		uint8_t* GetData();
	private:
		int m_Width;
		int m_Height;
		int m_NrChannels;
		uint8_t* m_Data;
	};
	
	class File {
	public:
		static std::string LoadTextFile(const std::string& path);
		static Image LoadImage(const std::string& path);
	private:
		static std::string s_WorkingDirectory;
	};
}