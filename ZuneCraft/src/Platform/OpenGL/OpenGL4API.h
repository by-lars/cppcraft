#pragma once
#include "Graphics/RenderAPI.h"
#include <vector>

namespace ZuneCraft {

	class OpenGL4API : public RenderAPI {
	struct GLShader {
		uint32_t Id;
	};

	struct GLBuffer {
		uint32_t Id;
		uint32_t VertexArray;
		uint32_t Type;
		uint32_t AttributeCount;
	};

	struct GLRenderTarget {
		uint32_t FrameBuffer;
	};

	struct GLTexture {
		uint32_t Id;
		uint32_t Width;
		uint32_t Height;
		GLenum Format;
		GLenum DataType;
	};

	public:
		OpenGL4API();
		~OpenGL4API();

		HShader CreateShader(const std::string& vertex, const std::string& fragment, const std::vector<std::string>& attributes) override;
		void BindShader(HShader hShader) override;
		void SetShaderUniform(HShader hShader, const std::string& name, const glm::vec3& value) override;
		void SetShaderUniform(HShader hShader, const std::string& name, const glm::mat4& value) override;
		void SetShaderUniform(HShader hShader, const std::string& name, int value) override;

		HBuffer CreateBuffer(uint32_t size, BufferType type, BufferUsage usage) override;
		void BindBuffer(HBuffer hBuffer) override;
		void SetBufferLayout(HBuffer buffer, std::initializer_list<BufferElement> elements, HBuffer hParrentBuffer = HBuffer::Invalid()) override;
		void BufferData(HBuffer hBuffer, uint32_t sizeInBytes, uint32_t offset, void* data) override;

		HTexture CreateTexture(uint32_t width, uint32_t height, TextureFormat format, DataType dataType, ClampMode clampMode, FilterMode filterMode) override;
		void BindTexture(HTexture hTexture) override;
		void UploadTextureData(HTexture hTexture, void* data) override;

		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;

		void DrawArrays(DrawMode mode, uint32_t offset, uint32_t count) override;
		void DrawArraysInstanced(DrawMode mode, uint32_t offset, uint32_t count, uint32_t instanceCount) override;

	private:
		std::vector<GLBuffer> m_Buffers;
		std::vector<GLShader> m_Shaders;
		std::vector<GLTexture> m_Textures;
	};
}