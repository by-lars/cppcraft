#include "Core/Base.h"
#include "Platform/OpenGL/GLVertexLayout.h"
#include "Platform/OpenGL/GL.h"

namespace ZuneCraft {
	//GL3 Vertex Layout
	GLVertexLayout::GLVertexLayout() {
	#ifdef ZC_PLATFORM_WIN32
		glGenVertexArrays(1, &m_Handle);
	#endif
		m_AttribCounter = 0;
		ZC_DEBUG("Created GLVertexLayout");
	}

	void GLVertexLayout::AddFormat(GPUStorage* storage, StorageFormat format, StorageUsage usage) {
	#ifdef ZC_PLATFORM_WIN32
		Bind();
		storage->Bind();

		switch (format) {
		case StorageFormat::FLOAT_VEC3:
			glVertexAttribPointer(m_AttribCounter, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (const void*)0);
			glEnableVertexAttribArray(m_AttribCounter);
			glVertexAttribDivisor(m_AttribCounter, usage == StorageUsage::INSTANCE_DATA);
			break;

		case StorageFormat::FLOAT_VEC4:
			glVertexAttribPointer(m_AttribCounter, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*)0);
			glEnableVertexAttribArray(m_AttribCounter);
			glVertexAttribDivisor(m_AttribCounter, usage == StorageUsage::INSTANCE_DATA);
			break;

		case StorageFormat::UBYTE_VEC4:
			glVertexAttribPointer(m_AttribCounter, 4, GL_UNSIGNED_BYTE, GL_FALSE, 4, (const void*)0);
			glEnableVertexAttribArray(m_AttribCounter);
			glVertexAttribDivisor(m_AttribCounter, usage == StorageUsage::INSTANCE_DATA);
			break;

		case StorageFormat::UBYTE_VEC4_VEC4:
			glVertexAttribPointer(m_AttribCounter, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, (const void*)0);
			glEnableVertexAttribArray(m_AttribCounter);
			glVertexAttribDivisor(m_AttribCounter, usage == StorageUsage::INSTANCE_DATA);

			glVertexAttribPointer(m_AttribCounter + 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, (const void*)4);
			glEnableVertexAttribArray(m_AttribCounter + 1);
			glVertexAttribDivisor(m_AttribCounter + 1, usage == StorageUsage::INSTANCE_DATA);
			m_AttribCounter++; //Extra increment because 2x Vec4
			break;
		default: ZC_FATAL_ERROR("Unknown Buffer Format"); break;
		}

		m_AttribCounter++;
	#endif
	}

	void GLVertexLayout::Bind() {
	#ifdef ZC_PLATFORM_WIN32
		glBindVertexArray(m_Handle);
	#endif
	}

	//GLES2 VertexLayout
	GLES2VertexLayout::GLES2VertexLayout() {
		ZC_DEBUG("Created GLES2VertexLayout");
	}

	void GLES2VertexLayout::AddFormat(GPUStorage* storage, StorageFormat format, StorageUsage usage) {
		ZC_ASSERT(usage != StorageUsage::INSTANCE_DATA, "StorageUsage::INSTANCE_DATA not supported by GLES2, used ShaderStorage instead");
		
		Layout layout;
		layout.Storage = storage;

		switch (format) {
		case StorageFormat::FLOAT_VEC3:
			layout.Count = 3;
			layout.Type = GL_FLOAT;
			layout.Stride = sizeof(float) * 3;
			layout.Offset = 0;
			m_Layout.push_back(layout);
			break;

		case StorageFormat::FLOAT_VEC4:
			layout.Count = 4;
			layout.Type = GL_FLOAT;
			layout.Stride = sizeof(float) * 4;
			layout.Offset = 0;
			m_Layout.push_back(layout);
			break;

		case StorageFormat::UBYTE_VEC4:
			layout.Count = 4;
			layout.Type = GL_UNSIGNED_BYTE;
			layout.Stride = 4;
			layout.Offset = 0;
			m_Layout.push_back(layout);
			break;

		case StorageFormat::UBYTE_VEC4_VEC4:
			layout.Count = 4;
			layout.Type = GL_UNSIGNED_BYTE;
			layout.Stride = 8;
			layout.Offset = 0;
			m_Layout.push_back(layout);
			
			layout.Offset = 4;
			m_Layout.push_back(layout);
			break;
		default: ZC_FATAL_ERROR("Unknown Buffer Format"); break;
		}
	}

	void GLES2VertexLayout::Bind() {
		for (int i = 0; i < m_Layout.size(); i++) {
			m_Layout[i].Storage->Bind();
			glVertexAttribPointer(i, m_Layout[i].Count, m_Layout[i].Type, GL_FALSE, m_Layout[i].Stride, (const void*)m_Layout[i].Offset);
			glEnableVertexAttribArray(i);
		}
	}

}