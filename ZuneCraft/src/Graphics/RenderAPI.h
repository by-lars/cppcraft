#pragma once
#include "Graphics/DeviceResources.h"
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

		struct Capabilities {
			bool ShaderCompiler;
			bool IndirectDrawing;
		};

		static RenderAPI* Create();
		static API GetAPI();
		static std::string GetAPIName(); 

		virtual const Capabilities& GetCapabilities() = 0;

		virtual HShader CreateShader(const std::string& vertexCode, const std::string& fragmentCode, const std::vector<std::string>& attributes) = 0;
		virtual HShader CreateShaderFromBinary(Binary& vertexBinary, Binary& fragmentBinary, const std::vector<std::string>& attributes) = 0;

		virtual void BindShader(HShader hShader) = 0;
		virtual void SetShaderUniform(HShader hShader, const std::string& name, const glm::vec3& value) = 0;
		virtual void SetShaderUniform(HShader hShader, const std::string& name, const glm::mat4& value) = 0;
		virtual void SetShaderUniform(HShader hShader, const std::string& name, int value) = 0;

		virtual HBuffer CreateBuffer(uint32_t size, BufferType type, BufferUsage usage) = 0;
		//std::initializer_list is not supported by c++03 :(
		virtual void SetBufferLayout(HBuffer hBuffer, HBuffer hParentBuffer, const std::vector<BufferElement>& elements) = 0;
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
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void DrawArrays(DrawMode mode, uint32_t offset, uint32_t count) = 0;
		//virtual void MultiDrawArrays() = 0;
		//virtual void DrawArraysInstanced(DrawMode mode, uint32_t offset, uint32_t count, uint32_t instanceCount) = 0;



		virtual void MultiDrawArraysIndirect(DrawMode mode, uint32_t nRenderCommands) = 0;
		//virtual void MultiDrawArrays();
	};
}