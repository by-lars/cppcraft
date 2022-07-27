#pragma once
#include "Graphics/DeviceResources.h"

#include <string>
#include <glm/glm.hpp>
#include <vector>

namespace ZuneCraft {
	class RenderAPI {
	public:
		enum class API {
			NONE = 0,
			OPENGL_ES_2,
			OPENGL_4
		};

		virtual ~RenderAPI() = default;

		static RenderAPI* Create();
		static API GetAPI();

		virtual HShader CreateShader(const std::string& vertexCode, const std::string& fragmentCode, const std::vector<std::string>& attributes) = 0;
		virtual void BindShader(HShader hShader) = 0;
		virtual void SetShaderUniform(HShader hShader, const std::string& name, const glm::vec3& value) = 0;
		virtual void SetShaderUniform(HShader hShader, const std::string& name, const glm::mat4& value) = 0;
		virtual void SetShaderUniform(HShader hShader, const std::string& name, int value) = 0;

		virtual HBuffer CreateBuffer(uint32_t size, BufferType type, BufferUsage usage) = 0;
		virtual void SetBufferLayout(HBuffer hBuffer, std::initializer_list<BufferElement> elements, HBuffer hParrentBuffer = HBuffer::Invalid()) = 0;
		virtual void BindBuffer(HBuffer hBuffer) = 0;
		virtual void BufferData(HBuffer hBuffer, uint32_t size, uint32_t offset, void* data) = 0;

		virtual HTexture CreateTexture(uint32_t width, uint32_t height, TextureFormat format, DataType dataType, ClampMode clampMode, FilterMode filterMode) = 0;
		virtual void BindTexture(HTexture hTexture) = 0;
		virtual void UploadTextureData(HTexture hTexture, void* data) = 0;

		//virtual HRenderTarget CreateRenderTarget(uint32_t width, uint32_t height) = 0;
		//virtual HTexture RenderTargetAddTextureAttachment() = 0;
		//virtual void RenderTargetAddBufferAttachment() = 0;
		//virtual void ResizeRenderTarget() = 0;

		virtual void Clear() = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;

		virtual void DrawArrays(DrawMode mode, uint32_t offset, uint32_t count) = 0;
		virtual void DrawArraysInstanced(DrawMode mode, uint32_t offset, uint32_t count, uint32_t instanceCount) = 0;
	private:
		static API s_API;
	};
}