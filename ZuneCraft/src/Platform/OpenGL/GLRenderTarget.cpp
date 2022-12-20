#include "Platform/OpenGL/GLRenderTarget.h"
#include "Platform/OpenGL/GLAPI.h"
#include "Platform/OpenGL/GL.h"

namespace ZuneCraft {
	GLRenderTarget::GLRenderTarget(GLuint width, GLuint height)
		: m_RenderBuffer(0), m_Width(width), m_Height(height) {
		
		glGenFramebuffers(1, &m_Handle);
		Bind();

		ZC_DEBUG("Created GLRenderTarget: " << width << "x" << height);
	}

	GLRenderTarget::GLRenderTarget(GLuint handle, GLuint width, GLuint height) {
		m_Handle = handle;
		m_Width = width;
		m_Height = height;
		m_Handle = handle;
		m_RenderBuffer = 0;
		Bind();

		ZC_DEBUG("Created Default GLRenderTarget with handle (" << m_Handle << "): " << width << "x" << height);
	}

	GLRenderTarget::~GLRenderTarget() {
		glDeleteFramebuffers(1, &m_Handle);
	}

	void GLRenderTarget::AddTexture(TextureFormat format) {
		GLuint texture;
		GLenum glFormat = OpenGLAPI::TextureFormatToNative(format);

		Bind();
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, glFormat, m_Width, m_Height, 0, glFormat, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GLenum(GL_COLOR_ATTACHMENT0 + m_ColorAttachements.size()), GL_TEXTURE_2D, texture, 0);

		m_ColorAttachements.push_back(texture);
	}

	void GLRenderTarget::AddRenderBuffer(TextureFormat format) {
		ZC_ASSERT(m_RenderBuffer == 0, "Only one RenderBuffer can be used per RenderTarget");

		GLenum glFormat = OpenGLAPI::TextureFormatToNative(format);

		Bind();
		glGenRenderbuffers(1, &m_RenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, glFormat, m_Width, m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
	}

	void GLRenderTarget::Finalize() {
		Bind();
		GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		if (m_ColorAttachements.size() > 0) {
			glDrawBuffers((GLsizei)m_ColorAttachements.size(), &attachments[0]);
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED: ZC_FATAL_ERROR("GL_FRAMEBUFFER_UNDEFINED"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
		case GL_FRAMEBUFFER_UNSUPPORTED: ZC_FATAL_ERROR("GL_FRAMEBUFFER_UNSUPPORTED"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
		}
	}

	void GLRenderTarget::Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
	}

	void GLRenderTarget::Resize(uint32_t width, uint32_t height) {
		ZC_ASSERT(false, "Not implemented");
	}

	uint32_t GLRenderTarget::GetNativeHandle() {
		return m_Handle;
	}
}