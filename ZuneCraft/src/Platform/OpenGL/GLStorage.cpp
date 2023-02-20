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
		m_Usage = usage;

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

	size_t GLVBOStorage::GetMaxCount() {
		return m_MaxCount;
	}

	uint32_t GLVBOStorage::GetNativeHandle() {
		return m_Handle;
	}

	static std::string ToType(GLenum e) {
		switch (e) {
		case GL_ARRAY_BUFFER: return "GL_ARRAY_BUFFER";
	#ifdef ZC_PLATFORM_WIN32
		case GL_VERTEX_ARRAY: return "GL_VERTEX_ARRAY";
		case GL_DRAW_INDIRECT_BUFFER: return "GL_DRAW_INDIRECT_BUFFER";
	#endif
		default: return "Unknown";
		}
	}

	void GLVBOStorage::Upload(uint32_t size, uint32_t offset, void* data) {
		if (size + offset > m_MaxCount) {
			Resize((size + m_MaxCount) * 1.5f);
		}

		glBindBuffer(m_Type, m_Handle);
		glBufferSubData(m_Type, offset * m_Stride, size * m_Stride, data);
	}

	void GLVBOStorage::Resize(uint32_t newSize) {
	#ifdef ZC_PLATFORM_WIN32
		GLuint tmpHandle = 0;

		glGenBuffers(1, &tmpHandle);
		glBindBuffer(m_Type, tmpHandle);
		glBufferData(m_Type, m_MaxCount * m_Stride, 0, GL_DYNAMIC_COPY);

		glBindBuffer(GL_COPY_WRITE_BUFFER, tmpHandle);
		glBindBuffer(m_Type, m_Handle);

		glCopyBufferSubData(m_Type, GL_COPY_WRITE_BUFFER, 0, 0, m_MaxCount * m_Stride);
		glBufferData(m_Type, newSize * m_Stride, 0, m_Usage);
		glCopyBufferSubData(GL_COPY_WRITE_BUFFER, m_Type, 0, 0, m_MaxCount * m_Stride);

		glDeleteBuffers(1, &tmpHandle);

		m_MaxCount = newSize;

		OnResize.Invoke(m_MaxCount);

		ZC_DEBUG("Growing buffer #" << m_Handle << "(" << ToType(m_Type) << ")" << " new size: " << m_MaxCount);
	#else
		ZC_DEBUG("GLVBOStroage::Resize is not implemented yet");
	#endif
	}

	//Shader Uniform 
	GLUniformStorage::GLUniformStorage(Ref<Shader>& shader, const std::string& location, StorageFormat format, GLuint initialSize) {
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

	size_t GLUniformStorage::GetMaxCount() {
		return m_MaxSize;
	}

	void GLUniformStorage::Resize(uint32_t newSize) {
		//Can't resize uniform storage
	}

}