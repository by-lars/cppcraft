#include "Platform/OpenGL/OpenGL4API.h"
#include "Core/Base.h"
#include "Graphics/GL.h"

#include <glm/gtc/type_ptr.hpp>

namespace ZuneCraft {
	#pragma region Constructor
	OpenGL4API::OpenGL4API() {
		m_Capabilities.ShaderCompiler = true;
		m_Capabilities.IndirectDrawing = true;

		const GLubyte* oglVersion = glGetString(GL_VERSION);
		const GLubyte* oglRenderer = glGetString(GL_RENDERER);
		const GLubyte* oglVendor = glGetString(GL_VENDOR);
		ZC_LOG("Running ", oglVersion, " on ", oglRenderer, " - ", oglVendor);
		
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
	}

	OpenGL4API::~OpenGL4API() {
		for (const GLShader& shader : m_Shaders) {
			ZC_DEBUG_ALLOC("Deleting Shader Id=", shader.Id);
			glDeleteProgram(shader.Id);
		}

		for (const GLBuffer& buffer : m_Buffers) {
			ZC_DEBUG_ALLOC("Deleting Buffer Id=", buffer.Id);
			glDeleteBuffers(1, &buffer.Id);
			ZC_DEBUG_ALLOC("Deleting Vertex Array Id=", buffer.VertexArray);
			glDeleteVertexArrays(1, &buffer.VertexArray);
		}


		for (const GLTexture& texture : m_Textures) {
			ZC_DEBUG_ALLOC("Deleting Texture Id=", texture.Id);
			glDeleteTextures(1, &texture.Id);
		}
	}

	const RenderAPI::Capabilities &OpenGL4API::GetCapabilities() {
		return m_Capabilities;
	}

	#pragma endregion

	#pragma region Type Conversion
	/*
	* Type Conversion
	*/
	static GLenum DrawModeToGLEnum(DrawMode mode) {
		switch (mode) {
		case DrawMode::TRIANGLES: return GL_TRIANGLES; break;
		case DrawMode::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP; break;
		case DrawMode::LINES: return GL_LINES; break;
		default: ZC_FATAL_ERROR("Unknown DrawMode"); break;
		}

		return GL_INVALID_ENUM;
	}

	static GLenum DataTypeToGLEnum(DataType dataType) {
		switch (dataType) {
		case DataType::FLOAT: return GL_FLOAT; break;
		case DataType::INT: return GL_INT; break;
		case DataType::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE; break;
		default: ZC_FATAL_ERROR("Unknown Buffer Element Type"); break;
		}

		return GL_INVALID_ENUM;
	}
	#pragma endregion
	
	#pragma region Shader
	/*
	 * SHADER
	 */
	static void CompileShader(GLint shader, const GLchar* source) {
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint success = false;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == false) {
			char log[1024];
			glGetShaderInfoLog(shader, 2048, 0, log);
			ZC_FATAL_ERROR("Failed to compile shader: ", shader, "\n", log);
		}
	}

	HShader OpenGL4API::CreateShader(const std::string& vertexCode, const std::string& fragmentCode, const std::vector<std::string>& attributes) {
		GLuint program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		ZC_DEBUG("Compiling Shader: ", " Id= ", program);

		CompileShader(vertex, vertexCode.c_str());
		CompileShader(fragment, fragmentCode.c_str());

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		GLint didLink = false;
		glGetProgramiv(program, GL_LINK_STATUS, &didLink);

		if (didLink == false) {
			char log[1024];
			glGetProgramInfoLog(program, 1024, 0, log);
			ZC_FATAL_ERROR("Failed to link shader: ", program, "\n", log);
		}

		m_Shaders.push_back(GLShader{ program });

		return HShader(m_Shaders.size() - 1);
	}

	HShader OpenGL4API::CreateShaderFromBinary(Binary& vertexBinary, Binary& fragmentBinary, const std::vector<std::string>& attributes) {
		ZC_ASSERT(false, "CreateShaderFromBinary is not supported by OpenGL4API, use CreateShader instead");
		return HShader::Invalid();
	}

	void OpenGL4API::BindShader(HShader hShader) {
		static HShader currentBinding = HShader::Invalid();

		//if (currentBinding != hShader) {
			GLShader shader = m_Shaders[(int)hShader];
			glUseProgram(shader.Id);
			currentBinding = hShader;
		//}
	}

	void OpenGL4API::SetShaderUniform(HShader hShader, const std::string& name, const glm::vec3& value) {
		BindShader(hShader);
		GLShader shader = m_Shaders[(int)hShader];
		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGL4API::SetShaderUniform(HShader hShader, const std::string& name, const glm::mat4& value) {
		BindShader(hShader);
		GLShader shader = m_Shaders[(int)hShader];
		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGL4API::SetShaderUniform(HShader hShader, const std::string& name, int value) {
		BindShader(hShader);
		GLShader shader = m_Shaders[(int)hShader];
		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniform1i(location, value);
	}
	#pragma endregion

	#pragma region Buffer
	/*
	 * BUFFER
	 */
	HBuffer OpenGL4API::CreateBuffer(uint32_t size, BufferType type, BufferUsage usage) {
		GLuint buffer = 0;
		glGenBuffers(1, &buffer);

		ZC_DEBUG_ALLOC("Created ID=", buffer, " of size=", size);

		GLenum bufferType;
		switch (type) {
		case BufferType::ARRAY: bufferType = GL_ARRAY_BUFFER; break;
		case BufferType::DRAW_INDIRECT_BUFFER: bufferType = GL_DRAW_INDIRECT_BUFFER; break;
		default: ZC_FATAL_ERROR("Unknown buffer type enum"); break;
		}

		GLenum glUsage;
		switch (usage) {
		case ZuneCraft::BufferUsage::STATIC_DRAW:	glUsage = GL_STATIC_DRAW;	break;
		case ZuneCraft::BufferUsage::STATIC_READ:	glUsage = GL_STATIC_READ;	break;
		case ZuneCraft::BufferUsage::STATIC_COPY:	glUsage = GL_STATIC_COPY;	break;
		case ZuneCraft::BufferUsage::DYNAMIC_DRAW:	glUsage = GL_DYNAMIC_DRAW;	break;
		case ZuneCraft::BufferUsage::DYNAMIC_READ:	glUsage = GL_DYNAMIC_READ;	break;
		case ZuneCraft::BufferUsage::DYNAMIC_COPY:	glUsage = GL_DYNAMIC_COPY;	break;
		case ZuneCraft::BufferUsage::STREAM_DRAW:	glUsage = GL_STREAM_DRAW;	break;
		case ZuneCraft::BufferUsage::STREAM_READ:	glUsage = GL_STREAM_READ;	break;
		case ZuneCraft::BufferUsage::STREAM_COPY:	glUsage = GL_STREAM_COPY;	break;
		default: ZC_FATAL_ERROR("Unknown buffer usage enum"); break;
		}


		glBindBuffer(bufferType, buffer);
		glBufferData(bufferType, size, 0, glUsage);

		m_Buffers.push_back(GLBuffer{ buffer, 0, bufferType, 0 });

		return HBuffer(m_Buffers.size() - 1);
	}

	void OpenGL4API::BindBuffer(HBuffer hBuffer) {

		GLBuffer buffer = m_Buffers[(int)hBuffer];
		if (buffer.VertexArray != 0) {
			glBindVertexArray(buffer.VertexArray);
		}
		else {
			glBindBuffer(buffer.Type, buffer.Id);
		}
	}

	void OpenGL4API::SetBufferLayout(HBuffer hBuffer, HBuffer hParentBuffer, const std::vector<BufferElement>& elements) {
		GLBuffer& buffer = m_Buffers[(int)hBuffer];
		int attrib = 0;
		int offset = 0;
		int stride = 0;

		//Create VAO
		if (hParentBuffer == HBuffer::Invalid()) {
			glGenVertexArrays(1, &buffer.VertexArray);
			ZC_DEBUG_ALLOC("Created VAO ID=", buffer.VertexArray);
		}
		else {
			//Use VAO of another buffer, if one is provided
			GLBuffer parrentBuffer = m_Buffers[(int)hParentBuffer];
			buffer.VertexArray = parrentBuffer.VertexArray;
			attrib = parrentBuffer.AttributeCount;
			ZC_ASSERT(parrentBuffer.AttributeCount > 0, "Parrent Buffer has to be setup first.");
			ZC_DEBUG("Attatching to VAO ID=", buffer.VertexArray, " from provided buffer");
		}

		//Bind VAO and Buffer
		glBindVertexArray(buffer.VertexArray);
		glBindBuffer(buffer.Type, buffer.Id);

		ZC_DEBUG("Setting Buffer Layout for buffer: ", buffer.Id);

		//Calculate the stride over all elements
		for (const BufferElement& element : elements) {
			stride += element.GetSizeInBytes();
		}

		//Set the buffer layout
		for (const BufferElement& element : elements) {
			GLenum attribType = DataTypeToGLEnum(element.Type);

			if (element.IsIntegerType()) {
				glVertexAttribIPointer(attrib, element.Count, attribType, stride, (const void*)offset);
			}
			else {
				glVertexAttribPointer(attrib, element.Count, attribType, GL_FALSE, stride, (const void*)offset);
			}

			glEnableVertexAttribArray(attrib);

			if (element.Divisor) {
				glVertexAttribDivisor(attrib, element.Divisor);
			}

			attrib += 1;
			offset += element.GetSizeInBytes();
		}

		buffer.AttributeCount = attrib;
	}

	void OpenGL4API::BufferData(HBuffer hBuffer, uint32_t size, uint32_t offset, void* data) {
		GLBuffer buffer = m_Buffers[(int)hBuffer];
		glBindBuffer(buffer.Type, buffer.Id);
		glBufferSubData(buffer.Type, offset, size, data);
		ZC_DEBUG("Uploading ", size, " bytes at offset ", offset, " to buffer #", buffer.Id);
	}
	#pragma endregion

	#pragma region Textures
	/*
	* Textures
	*/
	HTexture OpenGL4API::CreateTexture(uint32_t width, uint32_t height, TextureFormat format, DataType dataType, ClampMode clampMode, FilterMode filterMode) {
		GLuint id = 0;
		glGenTextures(1, &id);

		GLenum glDataType = DataTypeToGLEnum(dataType);
		GLenum glClampMode = GL_INVALID_ENUM;
		GLenum glMinFilterMode = GL_INVALID_ENUM;
		GLenum glMagFilterMode = GL_INVALID_ENUM;
		GLenum glTextureFormat = GL_INVALID_ENUM;

		switch (clampMode) {
		case ClampMode::REPEAT: glClampMode = GL_REPEAT; break;
		case ClampMode::CLAMP_TO_EDGE: glClampMode = GL_CLAMP_TO_EDGE; break;
		default: ZC_FATAL_ERROR("Unkown Clamp Mode"); break;
		}

		switch (filterMode) {
		case FilterMode::NEAREST: 
			glMinFilterMode = GL_NEAREST_MIPMAP_NEAREST;
			glMagFilterMode = GL_NEAREST;
			break;

		case FilterMode::LINEAR: 
			glMinFilterMode = GL_LINEAR_MIPMAP_LINEAR;
			glMagFilterMode = GL_LINEAR;
			break;

		default: ZC_FATAL_ERROR("Unkown Filter Mode"); break;
		}

		switch (format) {
		case TextureFormat::RGB: 
			glTextureFormat = GL_RGB; 
			break;

		case TextureFormat::RGBA: 
			glTextureFormat = GL_RGBA; 
			break;

		default: ZC_FATAL_ERROR("Unkown Texture Format"); break;
		}

		m_Textures.push_back(GLTexture{ id, width, height, glTextureFormat, glDataType});
		HTexture textureHandle(m_Textures.size() - 1);
		BindTexture(textureHandle);

		glTexImage2D(GL_TEXTURE_2D, 0, glTextureFormat, width, height, 0, glTextureFormat, glDataType, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilterMode);

		return textureHandle;
	}

	void OpenGL4API::UploadTextureData(HTexture hTexture, void* data) {
		BindTexture(hTexture);
		GLTexture texture = m_Textures[(int)hTexture];
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.Width, texture.Height, texture.Format, texture.DataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	void OpenGL4API::BindTexture(HTexture hTexture) {
		static HTexture currentBinding = HTexture::Invalid();

		//if (currentBinding != hTexture) {
			GLTexture texture = m_Textures[(int)hTexture];
			glBindTexture(GL_TEXTURE_2D, texture.Id);
			currentBinding = hTexture;
		//}
	}
	#pragma endregion

	#pragma region State
	/*
	* State
	*/
	void OpenGL4API::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void OpenGL4API::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGL4API::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}
	#pragma endregion

	#pragma region Draw Commands
	/*
	* Draw Commands
	*/
	void OpenGL4API::DrawArrays(DrawMode mode, uint32_t offset, uint32_t count) {
		GLenum glMode = DrawModeToGLEnum(mode);
		glDrawArrays(glMode, offset, count);
	}

	void OpenGL4API::MultiDrawArraysIndirect(DrawMode mode, uint32_t nRenderCommands) {
		GLenum glMode = DrawModeToGLEnum(mode);
		glMultiDrawArraysIndirect(glMode, 0, nRenderCommands, 0);
	}

	#pragma endregion
}