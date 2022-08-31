#pragma once
#include "Platform/OpenGL/OpenGLCommon.h"
#include "Graphics/RenderAPI.h"
#include "Core/Base.h"
#include "Data/Handle.h"
#include "Data/DenseArray.h"

#include <vector>

namespace ZuneCraft {
	struct GLES2BufferElement {
		GLenum DataType;
		GLuint Offset;
		GLuint Count;
	};

	struct GLES2Buffer {
		GLuint Id;
		GLsizei Stride;
		GLsizei Count;
		std::vector<GLES2BufferElement> VertexLayout;
	};

	class OpenGLES2API : public RenderAPI {
	public:
		OpenGLES2API();
		~OpenGLES2API();

		Id ShaderCreate(const std::string& assetName) override;
		void SetShaderUniform(Id hShader, const std::string& name, const glm::vec3& value) override;
		void SetShaderUniform(Id hShader, const std::string& name, const glm::mat4& value) override;
		void SetShaderUniform(Id hShader, const std::string& name, int value) override;

		Id StorageCreate(BufferSpec& spec) override;
		void StorageUpload(Id hBuffer, uint32_t size, uint32_t offset, void* data) override;

		Id TextureCreate(const TextureSpec& spec) override;
		void TextureBind(Id hTexture) override;
		void TextureUploadData(Id hTexture, void* data) override;

		Id RenderTargetCreate(const FramebufferSpec& spec) override;
		void RenderTargetBind(Id hRenderTarget) override;

		Id RenderCommandPush(const RenderCommand& command) override;
		void RenderCommandErase(Id hRenderCommand) override;
		void Flush() override;

		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void DrawArrays(Id hShader, Id hBuffer, DrawMode mode, uint32_t offset, uint32_t count) override;
		void Draw(Id hShader, Id hBuffer, DrawMode mode) override;

	private:
		Id ShaderFromBinary(const std::string& assetName);
		Id ShaderFromSource(const std::string& assetName);

		void BindBuffer(Id hBuffer);
		Id CreateBuffer(const BufferSpec& spec);
		Id CreateShaderUniform(const BufferSpec& spec, const std::string& name);

		std::vector<GLES2Buffer> m_Buffers;
		std::vector<GLShader> m_Shaders;
		std::vector<GLUniform> m_ShaderUniforms;
		std::vector<GLTexture> m_Textures;
		std::vector<GLRenderTarget> m_RenderTargets;

		DenseArray<RenderCommand> m_RenderCommands;
	};
}