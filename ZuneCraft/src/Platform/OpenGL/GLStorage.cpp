#include "Graphics/RenderAPI.h"
#include "Platform/OpenGL/GLStorage.h"
#include "Platform/OpenGL/GL.h"
#include <sstream>

namespace ZuneCraft {
	//Vertex Buffer Object
	GLVBOStorage::GLVBOStorage(GLenum type, GLuint initialCount, GLuint stride, GLenum usage, void* initialData) {
		m_MaxCount = initialCount;
		m_Stride = stride;
		m_Type = type;

		glGenBuffers(1, &m_Handle);
		glBindBuffer(m_Type, m_Handle);
		glBufferData(m_Type, initialCount * stride, initialData, usage);
		
		ZC_DEBUG("Created GLVBOStorage - initialSize=" << initialCount * stride << "bytes");
	}

	GLVBOStorage::~GLVBOStorage() {
		glDeleteBuffers(1, &m_Handle);
	}

	void GLVBOStorage::Bind() {
		glBindBuffer(m_Type, m_Handle);
	}

	uint32_t GLVBOStorage::GetNativeHandle() {
		return m_Handle;
	}

	void GLVBOStorage::Upload(uint32_t size, uint32_t offset, void* data) {
		if (size + offset < m_MaxCount) {
			glBindBuffer(m_Type, m_Handle);
			glBufferSubData(m_Type, offset * m_Stride, size * m_Stride, data);
		}
		else {
			//TODO: Grow Buffer
			ZC_ASSERT(false, "Out of mem - Resize not implemented")
		}
	}

	//Shader Uniform 
	GLUniformStorage::GLUniformStorage(Shader* shader, const std::string& location, StorageFormat format, GLuint initialSize) {
		m_MaxSize = initialSize;
		m_Shader = shader;
		m_Format = format;
		m_UniformName = location;

		glUseProgram(m_Shader->GetNativeHandle());
		m_Location = glGetUniformLocation(m_Shader->GetNativeHandle(), location.c_str());
		ZC_DEBUG("Created GLUniformStorage: '" << location << "'");
	}

	void GLUniformStorage::Upload(uint32_t size, uint32_t offset, void* data) {
		GLint location = m_Location;
		m_Shader->Bind();

		if (offset + size > m_MaxSize) {
			ZC_WARN("Can't upload... uniform too small!");
			return;
		}

		if (offset != 0) {
			std::stringstream ss;
			ss << m_UniformName << "[" << offset << "]";
			std::string name = ss.str();
			location = glGetUniformLocation(m_Shader->GetNativeHandle(), name.c_str());
		}

		switch (m_Format) {
		case StorageFormat::FLOAT_VEC3: glUniform3fv(location, size, (const GLfloat*)data); break;
		case StorageFormat::FLOAT_VEC4: glUniform4fv(location, size, (const GLfloat*)data); break;
		case StorageFormat::INT_VEC1: glUniform1iv(location, size, (const GLint*)data); break;

		default: ZC_FATAL_ERROR("Unknown Uniform Format"); 
		}
	}

	uint32_t GLUniformStorage::GetNativeHandle() {
		return m_Location;
	}

	void GLUniformStorage::Bind() {
		m_Shader->Bind();
	}
}