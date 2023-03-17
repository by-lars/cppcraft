#pragma once
#include "Core/Base.h"
#include "Graphics/RenderAPI.h"

#include <vector>

namespace ZuneCraft {
	class GLRenderTarget : public RenderTarget {
	public:
		GLRenderTarget(GLuint height, GLuint width);
		GLRenderTarget(GLuint handle, GLuint width, GLuint height);
		~GLRenderTarget();

		void AddTexture(TextureFormat format) override;
		void AddRenderBuffer(TextureFormat format) override;
		void Finalize() override;
		void Resize(uint32_t width, uint32_t height) override;
		void Bind() override;
		uint32_t GetNativeHandle() override;

	private:
		GLuint m_RenderBuffer;
		std::vector<GLuint> m_ColorAttachements;

		GLuint m_Handle;
		GLuint m_Width;
		GLuint m_Height;
	};
}