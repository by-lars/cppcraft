#include "Core/Base.h"
#include "Platform/OpenGL/OpenGLES2API.h"
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/GL.h"
#include <Windows.h>

namespace ZuneCraft {
#pragma region Constructor
	OpenGLES2API::OpenGLES2API() {
		ZC_LOG("Initializing OpenGLES 2.0 API");

		m_Capabilities.IndirectDrawing = false;

		#ifdef ZC_PLATFORM_ZUNE
			m_Capabilities.ShaderCompiler = false;	
		#else
			m_Capabilities.ShaderCompiler = true;
		#endif

		std::string oglVersion((const char*)glGetString(GL_VERSION));
		std::string oglRenderer((const char*)glGetString(GL_RENDERER));
		std::string oglVendor((const char*)glGetString(GL_VENDOR));

		ZC_LOG("Running " << oglVersion << " on " << oglRenderer << " - " << oglVendor);
		
		m_BatchedDrawing.VertCount = 0;

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

	}

	OpenGLES2API::~OpenGLES2API() {
		
		for(int i = 0; i < m_Shaders.size(); i++) {
			ZC_DEBUG_ALLOC("Deleting Shader Id=" << m_Shaders[i].Id);
			glDeleteProgram(m_Shaders[i].Id);
		}

		for (int i = 0; i < m_Buffers.size(); i++) {
			ZC_DEBUG_ALLOC("Deleting Buffer Id=" << m_Buffers[i].Id);
			glDeleteBuffers(1, &m_Buffers[i].Id);
		}

		for (int i = 0; i < m_Textures.size(); i++) {
			ZC_DEBUG_ALLOC("Deleting Texture Id=" << m_Textures[i].Id);
			glDeleteTextures(1, &m_Textures[i].Id);
		}
	}

	const RenderAPI::Capabilities& OpenGLES2API::GetCapabilities() {
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
	#ifdef ZC_PLATFORM_WIN32
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint success = false;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == false) {
			char log[1024];
			glGetShaderInfoLog(shader, 2048, 0, log);
			ZC_FATAL_ERROR("Failed to compile shader: " << shader << "\n" << log);
		}
	#endif 
	}

	HShader OpenGLES2API::CreateShader(const std::string& vertexCode, const std::string& fragmentCode, const std::vector<std::string>& attributes) {
		GLuint program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		ZC_DEBUG("Compiling Shader: " << " Id= " << program);

		CompileShader(vertex, vertexCode.c_str());
		CompileShader(fragment, fragmentCode.c_str());

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);


		for (int i = 0; i < attributes.size(); i++) {
			glBindAttribLocation(program, i, attributes[i].c_str());
		}

		glLinkProgram(program);

		GLint didLink = false;
		glGetProgramiv(program, GL_LINK_STATUS, &didLink);

		if (didLink == false) {
			char log[1024];
			glGetProgramInfoLog(program, 1024, 0, log);
			ZC_FATAL_ERROR("Failed to link shader: " << program << "\n" << log);
		}

		GLShader shader; shader.Id = program; 
		m_Shaders.push_back(shader);

		return HShader(m_Shaders.size() - 1);
	}

	HShader OpenGLES2API::CreateShaderFromBinary(Binary& vertexBinary, Binary& fragmentBinary, const std::vector<std::string>& attributes) {
		GLuint program = glCreateProgram();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderBinary(1, &vertexShader, GL_NVIDIA_PLATFORM_BINARY_NV, vertexBinary.Data, vertexBinary.Size);
		glShaderBinary(1, &fragmentShader, GL_NVIDIA_PLATFORM_BINARY_NV, fragmentBinary.Data, fragmentBinary.Size);

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		for (int i = 0; i < attributes.size(); i++) {
			ZC_DEBUG("Bound " << attributes[i] << " to location " << i);
			glBindAttribLocation(program, i, attributes[i].c_str());
		}

		glLinkProgram(program);

		GLint didLink = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &didLink);
		if (didLink == false) {
			char info[1024];
			GLsizei len = 0;
			glGetProgramInfoLog(program, 1024, &len, info);
			ZC_FATAL_ERROR("Could not link shaders:" << info);
		}


		GLShader shader; shader.Id = program;
		m_Shaders.push_back(shader);

		return HShader(m_Shaders.size() - 1);
	}

	void OpenGLES2API::BindShader(HShader hShader) {
		static HShader currentBinding = HShader::Invalid();

		//if (currentBinding != hShader) {
		GLShader shader = m_Shaders[(int)hShader];
		glUseProgram(shader.Id);
		currentBinding = hShader;
		//}
	}

	void OpenGLES2API::SetShaderUniform(HShader hShader, const std::string& name, const glm::vec3& value) {
		BindShader(hShader);
		GLShader shader = m_Shaders[(int)hShader];
		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLES2API::SetShaderUniform(HShader hShader, const std::string& name, const glm::mat4& value) {
		BindShader(hShader);
		GLShader shader = m_Shaders[(int)hShader];
		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLES2API::SetShaderUniform(HShader hShader, const std::string& name, int value) {
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
	HBuffer OpenGLES2API::CreateBuffer(uint32_t size, BufferType type, BufferUsage usage) {
		GLuint buffer = 0;
		glGenBuffers(1, &buffer);

		ZC_DEBUG("Created ID=" << buffer << " of size=" << size);

		GLenum bufferType;
		switch (type) {
		case BufferType::ARRAY: bufferType = GL_ARRAY_BUFFER; break;
		default: ZC_FATAL_ERROR("Unknown buffer type enum"); break;
		}

		GLenum glUsage;
		switch (usage) {
		case ZuneCraft::BufferUsage::STATIC_DRAW:	glUsage = GL_STATIC_DRAW;	break;
		case ZuneCraft::BufferUsage::DYNAMIC_DRAW:	glUsage = GL_DYNAMIC_DRAW;	break;
		case ZuneCraft::BufferUsage::STREAM_DRAW:	glUsage = GL_STREAM_DRAW;	break;
		default: ZC_FATAL_ERROR("Unknown buffer usage enum"); break;
		}


		glBindBuffer(bufferType, buffer);
		glBufferData(bufferType, size, 0, glUsage);

		GLES2Buffer glBuffer;
		glBuffer.Id = buffer;
		glBuffer.Type = bufferType;
		m_Buffers.push_back(glBuffer);

		return HBuffer(m_Buffers.size() - 1);
	}

	void OpenGLES2API::BindBuffer(HBuffer hBuffer) {
		GLES2Buffer buffer = m_Buffers[(int)hBuffer];
		glBindBuffer(buffer.Type, buffer.Id);

		for (int i = 0; i < buffer.VertexLayout.size(); i++) {
			glVertexAttribPointer(i, buffer.VertexLayout[i].Count, buffer.VertexLayout[i].DataType, false, buffer.Stride, (const void*)buffer.VertexLayout[i].Offset);
			glEnableVertexAttribArray(i);
		}

	}

	void OpenGLES2API::SetBufferLayout(HBuffer hBuffer, HBuffer hParentBuffer, const std::vector<BufferElement>& elements) {
		ZC_ASSERT(hParentBuffer == HBuffer::Invalid(), "VAOs are not supported by GLES2.0");
		uint32_t offset = 0;

		GLES2Buffer& buffer = m_Buffers[(int)hBuffer];
		ZC_DEBUG("Setting Buffer Layout for buffer: " << buffer.Id);


		for (int i = 0; i < elements.size(); i++) {
			GLES2BufferElement e;
			e.Count = elements[i].Count;
			e.DataType = DataTypeToGLEnum(elements[i].Type);
			e.Offset = offset;

			buffer.VertexLayout.push_back(e);

			offset += elements[i].GetSizeInBytes();
		}

		buffer.Stride = offset;
	}

	void OpenGLES2API::BufferData(HBuffer hBuffer, uint32_t size, uint32_t offset, void* data) {
		GLES2Buffer buffer = m_Buffers[(int)hBuffer];
		glBindBuffer(buffer.Type, buffer.Id);
		glBufferSubData(buffer.Type, offset, size, data);
		ZC_DEBUG("Uploading " << size << " bytes at offset " << offset << " to buffer #" << buffer.Id);
	}
#pragma endregion

#pragma region Textures
	/*
	* Textures
	*/
	HTexture OpenGLES2API::CreateTexture(uint32_t width, uint32_t height, TextureFormat format, DataType dataType, ClampMode clampMode, FilterMode filterMode) {
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

		GLTexture texture;
		texture.Id = id;
		texture.Width = width;
		texture.Height = height;
		texture.Format = glTextureFormat;
		texture.DataType = glDataType;
		m_Textures.push_back(texture);

		HTexture textureHandle(m_Textures.size() - 1);
		BindTexture(textureHandle);

		glTexImage2D(GL_TEXTURE_2D, 0, glTextureFormat, width, height, 0, glTextureFormat, glDataType, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilterMode);

		return textureHandle;
	}

	void OpenGLES2API::UploadTextureData(HTexture hTexture, void* data) {
		BindTexture(hTexture);
		GLTexture texture = m_Textures[(int)hTexture];
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.Width, texture.Height, texture.Format, texture.DataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	void OpenGLES2API::BindTexture(HTexture hTexture) {
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
	void OpenGLES2API::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void OpenGLES2API::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLES2API::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}
#pragma endregion

#pragma region Draw Commands
	/*
	* Draw Commands
	*/
	void OpenGLES2API::PushRenderCommand(const RenderCommand& command) {
		m_BatchedDrawing.First.push_back(command.First);
		m_BatchedDrawing.Count.push_back(command.Count);
		m_BatchedDrawing.VertCount += command.Count;
	}

	void OpenGLES2API::Flush() {

	}

	void OpenGLES2API::DrawArrays(DrawMode mode, uint32_t offset, uint32_t count) {
		GLenum glMode = DrawModeToGLEnum(mode);
		glDrawArrays(glMode, offset, count);
	}

	void OpenGLES2API::MultiDrawArrays(DrawMode mode) {
		GLenum glMode = DrawModeToGLEnum(mode);

		glDrawArrays(glMode, 0, m_BatchedDrawing.VertCount);
		//glMultiDrawArrays(glMode, &m_BatchedDrawing.First[0], &m_BatchedDrawing.Count[0], m_BatchedDrawing.First.size());
	}

#pragma endregion
}