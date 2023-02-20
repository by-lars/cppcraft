#pragma once
#include "Graphics/DeviceResources.h"
#include "Graphics/RenderAPI.h"
#include <vector>

namespace ZuneCraft {
	class GLDirectCommandQueue : public CommandQueue {
	public:
		GLDirectCommandQueue(GLenum drawMode);
		void Push(const RenderCommand& cmd) override;
		void Clear() override;
		void Draw() override;

	private:
		GLenum m_DrawMode;
		std::vector<RenderCommand> m_Commands;
	};

	class GLIndirectCommandQueue : public CommandQueue {
	public:
		GLIndirectCommandQueue(RenderAPI* device, GLenum drawMode);
		void Push(const RenderCommand& cmd) override;
		void Clear() override;
		void Draw() override;

	private:
		GLenum m_DrawMode;
		Ref<GPUStorage> m_Buffer;
		GLuint m_Count;
	};
}