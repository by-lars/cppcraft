#include "Platform/OpenGL/GLTexture.h"
#include "Platform/OpenGL/GL.h"

namespace ZuneCraft {
	GLTexture::GLTexture(GLuint width, GLuint height, GLenum format, GLenum minFilterMode, GLenum magFilterMode)
	: m_Width(width), m_Height(height), m_Format(format) {

		glGenTextures(1, &m_Handle);

		glBindTexture(GL_TEXTURE_2D, m_Handle);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode);

		ZC_DEBUG("Created GLTexture: " << width << "x" << height);
	}

	void GLTexture::Bind() {
		glBindTexture(GL_TEXTURE, m_Handle);
	}

	void GLTexture::Upload(void* data) {
		glBindTexture(GL_TEXTURE_2D, m_Handle);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	uint32_t GLTexture::GetNativeHandle() {
		return m_Handle;
	}
}