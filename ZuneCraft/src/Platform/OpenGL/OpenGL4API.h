#pragma once
#include "Graphics/RenderAPI.h"
#include "Platform/OpenGL/OpenGLCommon.h"
#include <vector>

namespace ZuneCraft {
	class OpenGL4API : public RenderAPI {
	public:
		OpenGL4API();
		~OpenGL4API();

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

		void RenderCommandPush(const RenderCommand& command) override;
		void Flush() override;

		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void DrawArrays(Id hShader, Id hBuffer, DrawMode mode, uint32_t offset, uint32_t count) override;
		void Draw(Id hShader, Id hBuffer, DrawMode mode) override;

	private:

		std::vector<GLBuffer> m_Buffers;
		std::vector<GLShader> m_Shaders;
		std::vector<GLTexture> m_Textures;
		std::vector<GLRenderTarget> m_RenderTargets;

		struct {
			std::vector<RenderCommand> Commands;
			Id CommandBuffer;
			bool Invalidated;
		} m_BatchedDrawing;

	};
}