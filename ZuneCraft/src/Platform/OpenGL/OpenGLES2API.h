#pragma once
#include "Graphics/RenderAPI.h"
#include "Platform/OpenGL/OpenGLCommon.h"
#include "Core/Base.h"
#include <vector>

namespace ZuneCraft {
	struct GLES2BufferElement {
		GLenum DataType;
		GLuint Offset;
		GLuint Count;
	};

	struct GLES2Buffer {
		GLuint Id;
		GLenum Type;
		GLsizei Stride;
		std::vector<GLES2BufferElement> VertexLayout;
	};

	class OpenGLES2API : public RenderAPI {
	public:
		OpenGLES2API();
		~OpenGLES2API();

		const Capabilities& GetCapabilities() override;

		HShader CreateShader(const std::string& vertex, const std::string& fragment, const std::vector<std::string>& attributes) override;
		HShader CreateShaderFromBinary(Binary& vertexBinary, Binary& fragmentBinary, const std::vector<std::string>& attributes) override;
		void BindShader(HShader hShader) override;
		void SetShaderUniform(HShader hShader, const std::string& name, const glm::vec3& value) override;
		void SetShaderUniform(HShader hShader, const std::string& name, const glm::mat4& value) override;
		void SetShaderUniform(HShader hShader, const std::string& name, int value) override;

		HBuffer CreateBuffer(uint32_t size, BufferType type, BufferUsage usage) override;
		void BindBuffer(HBuffer hBuffer) override;
		void SetBufferLayout(HBuffer hBuffer, HBuffer hParentBuffer, const std::vector<BufferElement>& elements) override;
		void BufferData(HBuffer hBuffer, uint32_t sizeInBytes, uint32_t offset, void* data) override;

		HTexture CreateTexture(uint32_t width, uint32_t height, TextureFormat format, DataType dataType, ClampMode clampMode, FilterMode filterMode) override;
		void BindTexture(HTexture hTexture) override;
		void UploadTextureData(HTexture hTexture, void* data) override;

		HRenderTarget CreateRenderTarget(uint32_t width, uint32_t height) override;
		void BindRenderTarget(HRenderTarget hRenderTarget) override;
		void RenderTargetAddTextureAttachment(HRenderTarget hRenderTarget, TextureFormat format, TextureFormat internalFormat, AttachementType attachementType) override;
		void RenderTargetAddBufferAttachment(HRenderTarget hRenderTarget, TextureFormat format, AttachementType attachementType) override;
		void FinalizeRenderTarget(HRenderTarget hRenderTarget) override;

		void PushRenderCommand(const RenderCommand& command) override;
		void Flush() override;

		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void DrawArrays(DrawMode mode, uint32_t offset, uint32_t count) override;
		void MultiDrawArrays(DrawMode mode) override;

	private:
		Capabilities m_Capabilities;

		std::vector<GLES2Buffer> m_Buffers;
		std::vector<GLShader> m_Shaders;
		std::vector<GLTexture> m_Textures;
		std::vector<GLRenderTarget> m_RenderTargets;

		struct {
			std::vector<GLint> First;
			std::vector<GLsizei> Count;
			GLsizei VertCount;
		} m_BatchedDrawing;

	};
}