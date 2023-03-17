#pragma once
#include "Core/Base.h"
#include "Graphics/RenderAPI.h"

namespace ZuneCraft {
	class GLTexture : public Texture {
	public:
		GLTexture(GLuint width, GLuint height, GLenum format, GLenum minFilterMode, GLenum magFilterMode);

		void Bind() override;
		void Upload(void* data) override;
		uint32_t GetNativeHandle() override;

	private:
		GLuint m_Handle;
		GLuint m_Width;
		GLuint m_Height;
		GLenum m_Format;
	};
}