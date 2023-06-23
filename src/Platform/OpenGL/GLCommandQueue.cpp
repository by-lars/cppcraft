#include "Platform/OpenGL/GLCommandQueue.h"
#include "Platform/OpenGL/GL.h"

namespace ZuneCraft {
	//Direct Command Queue
	GLDirectCommandQueue::GLDirectCommandQueue(GLenum drawMode) {
		m_DrawMode = drawMode;
		ZC_DEBUG("Created GLDirectCommandQueue");
	}

	void GLDirectCommandQueue::Push(const RenderCommand& cmd) {
		m_Commands.push_back(cmd);
	}

	void GLDirectCommandQueue::Clear() {
		m_Commands.clear();
	}

	void GLDirectCommandQueue::Draw() {
		for (size_t i = 0; i < m_Commands.size(); i++) {
			glDrawArrays(m_DrawMode, m_Commands[i].First, m_Commands[i].Count);
		}
	}

	//Indirect COmmand Queue
	GLIndirectCommandQueue::GLIndirectCommandQueue(RenderAPI* device, GLenum drawMode) {
		m_Buffer = device->CreateStorage(StorageUsage::DRAW_COMMAND, StorageFormat::DRAW_COMMAND, 512, 0);
		m_Count = 0;
		m_DrawMode = drawMode;
		ZC_DEBUG("Created GLIndirectCommandQueue");
	}

	void GLIndirectCommandQueue::Push(const RenderCommand& cmd) {
		m_Buffer->Upload(1, m_Count, (void*)&cmd);
		m_Count++;
	}

	void GLIndirectCommandQueue::Clear() {
		m_Count = 0;
	}

	void GLIndirectCommandQueue::Draw() {
		glMultiDrawArraysIndirect(m_DrawMode, 0, m_Count, 0);
	}

}