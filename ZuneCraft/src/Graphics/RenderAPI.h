#pragma once
#include "Graphics/DeviceResources.h"
#include "Data/Handle.h"
#include "Utility/Asset.h"
#include <string>
#include <glm/glm.hpp>
#include <vector>

namespace ZuneCraft {
	class RenderAPI {
	public:

		ZC_ENUM API {
			NONE = 0,
			OPENGL_ES_2,
			OPENGL_4
		};

		static RenderAPI* Create();
		static API GetAPI();
		static std::string GetAPIName(); 

		virtual Id ShaderCreate(const std::string& assetName) = 0;
		virtual void SetShaderUniform(Id hShader, const std::string& name, const glm::vec3& value) = 0;
		virtual void SetShaderUniform(Id hShader, const std::string& name, const glm::mat4& value) = 0;
		virtual void SetShaderUniform(Id hShader, const std::string& name, int value) = 0;

		virtual Id StorageCreate(BufferSpec& spec) = 0;
		virtual void StorageUpload(Id hBuffer, uint32_t size, uint32_t offset, void* data) = 0;

		virtual Id TextureCreate(const TextureSpec& spec) = 0;
		virtual void TextureBind(Id hTexture) = 0;
		virtual void TextureUploadData(Id hTexture, void* data) = 0;

		virtual Id RenderTargetCreate(const FramebufferSpec& spec) = 0;
		virtual void RenderTargetBind(Id hRenderTarget) = 0;

		virtual Id RenderCommandPush(const RenderCommand& command) = 0;
		virtual void RenderCommandErase(Id hRenderCommand) = 0;
		virtual void Flush() = 0;

		virtual void Clear() = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void DrawArrays(Id hShader, Id hBuffer, DrawMode mode, uint32_t offset, uint32_t count) = 0;
		virtual void Draw(Id hShader, Id hBuffer, DrawMode mode) = 0;
	};
}