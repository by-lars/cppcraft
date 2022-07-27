#pragma once
#include "Graphics/RenderAPI.h"
#include <vector>

namespace ZuneCraft {

	class OpenGLES2API : public RenderAPI {
	struct GLShader {
		uint32_t Id;
	};

	struct GLBuffer {
		uint32_t Id;
		uint32_t VertexArray;
		uint32_t Type;
		uint32_t AttributeCount;
	};

	public:
		OpenGLES2API();
		~OpenGLES2API();

		HShader CreateShader(const std::string& name, const std::string& vertex, const std::string& fragment) override;
		void BindShader(HShader hShader) override;

		HBuffer CreateBuffer(uint32_t size, BufferType type, BufferUsage usage) override;
		void BindBuffer(HBuffer hBuffer) override;
		void SetBufferLayout(HBuffer buffer, std::initializer_list<BufferElement> elements, HBuffer hParrentBuffer = HBuffer::Invalid()) override;
		void BufferData(HBuffer hBuffer, uint32_t sizeInBytes, uint32_t offset, void* data) override;

		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;

		void DrawArrays(DrawMode mode, uint32_t count, uint32_t offset) override;
		void DrawArraysInstanced(DrawMode mode, uint32_t count, uint32_t offset, uint32_t instanceCount) override;

	private:
		std::vector<GLBuffer> m_Buffers;
		std::vector<GLShader> m_Shaders;
	};
}