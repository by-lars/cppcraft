#include "Platform/OpenGL/OpenGL4API.h"
#include "Core/Base.h"
#include "Graphics/GL.h"

#include <glm/gtc/type_ptr.hpp>

namespace ZuneCraft {
	#pragma region Constructor
	OpenGL4API::OpenGL4API() {
		ZC_LOG("Initializing OpenGL 4.6 API");

		m_Capabilities.ShaderCompiler = true;
		m_Capabilities.IndirectDrawing = true;

		const GLubyte* oglVersion = glGetString(GL_VERSION);
		const GLubyte* oglRenderer = glGetString(GL_RENDERER);
		const GLubyte* oglVendor = glGetString(GL_VENDOR);
		ZC_LOG("Running " << oglVersion << " on " << oglRenderer << " - " << oglVendor);
		
		m_BatchedDrawing.Invalidated = false;
		m_BatchedDrawing.CommandBuffer = CreateBuffer(128 * sizeof(RenderCommand), BufferType::DRAW_INDIRECT_BUFFER, BufferUsage::DYNAMIC_DRAW);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
	}

	OpenGL4API::~OpenGL4API() {
		for (const GLShader& shader : m_Shaders) {
			ZC_DEBUG_ALLOC("Deleting Shader Id=" << shader.Id);
			glDeleteProgram(shader.Id);
		}

		for (const GLBuffer& buffer : m_Buffers) {
			ZC_DEBUG_ALLOC("Deleting Buffer Id=" << buffer.Id);
			glDeleteBuffers(1, &buffer.Id);
			ZC_DEBUG_ALLOC("Deleting Vertex Array Id=" << buffer.VertexArray);
			glDeleteVertexArrays(1, &buffer.VertexArray);
		}


		for (const GLTexture& texture : m_Textures) {
			ZC_DEBUG_ALLOC("Deleting Texture Id=" << texture.Id);
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

	static GLenum TextureFormatToGLEnum(TextureFormat format) {
		switch (format) {
		case TextureFormat::RGB: return GL_RGB; break;
		case TextureFormat::RGBA: return GL_RGBA; break;
		case TextureFormat::DEPTH_COMPONENT16: return GL_DEPTH_COMPONENT16; break;
		case TextureFormat::DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT24; break;
		case TextureFormat::DEPTH_COMPONENT32: return GL_DEPTH_COMPONENT32; break;
		default: ZC_FATAL_ERROR("Unkown Texture Format"); break;
		}
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
			ZC_FATAL_ERROR("Failed to compile shader: " << shader << "\n" << log);
		}
	}

	HShader OpenGL4API::CreateShader(const std::string& vertexCode, const std::string& fragmentCode, const std::vector<std::string>& attributes) {
		GLuint program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		ZC_DEBUG("Compiling Shader Id= " << program);

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
			ZC_FATAL_ERROR("Failed to link shader: " << program << "\n" << log);
		}

		m_Shaders.push_back(GLShader{ .Id = program });

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

		ZC_DEBUG_ALLOC("Created ID=" << buffer << " of size=" << size);

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

		m_Buffers.push_back(GLBuffer { 
			.Id = buffer, 
			.VertexArray = 0, 
			.Type = bufferType, 
			.AttributeCount = 0, 
			.Size = size 
		});

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
			ZC_DEBUG_ALLOC("Created VAO ID=" << buffer.VertexArray);
		}
		else {
			//Use VAO of another buffer, if one is provided
			GLBuffer parrentBuffer = m_Buffers[(int)hParentBuffer];
			buffer.VertexArray = parrentBuffer.VertexArray;
			attrib = parrentBuffer.AttributeCount;
			ZC_ASSERT(parrentBuffer.AttributeCount > 0, "Parrent Buffer has to be setup first.");
			ZC_DEBUG("Attatching to VAO ID=" << buffer.VertexArray << " from provided buffer");
		}

		//Bind VAO and Buffer
		glBindVertexArray(buffer.VertexArray);
		glBindBuffer(buffer.Type, buffer.Id);

		ZC_DEBUG("Setting Buffer Layout for buffer: " << buffer.Id);

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
		ZC_DEBUG("Uploading " << size << " bytes at offset " << offset << " to buffer #" << buffer.Id);
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
		GLenum glTextureFormat = TextureFormatToGLEnum(format);
		GLenum glClampMode = GL_INVALID_ENUM;
		GLenum glMinFilterMode = GL_INVALID_ENUM;
		GLenum glMagFilterMode = GL_INVALID_ENUM;

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

		m_Textures.push_back(GLTexture { 
			.Id = id, 
			.Width = width, 
			.Height = height, 
			.Format = glTextureFormat, 
			.DataType = glDataType
		});

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

	#pragma region FrameBuffer
	/*
	* FrameBuffer
	*/

	HRenderTarget OpenGL4API::CreateRenderTarget(uint32_t width, uint32_t height) {
		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		m_RenderTargets.push_back(GLRenderTarget{ .Id = fbo, .Width = width, .Height = height, .DepthAttachement = 0 });
		return HRenderTarget(m_RenderTargets.size() - 1);
	}

	void OpenGL4API::BindRenderTarget(HRenderTarget hRenderTarget) {
		if (hRenderTarget == HRenderTarget::Invalid()) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else {
			GLRenderTarget renderTarget = m_RenderTargets[(int)hRenderTarget];
			glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);
		}
	}

	void OpenGL4API::RenderTargetAddTextureAttachment(HRenderTarget hRenderTarget, TextureFormat format, TextureFormat internalFormat, AttachementType attachementType) {
		GLRenderTarget& renderTarget = m_RenderTargets[(int)hRenderTarget];
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);

		GLuint tex;
		GLenum texFormat = GL_INVALID_ENUM;
		GLenum texInternalFormat = GL_INVALID_ENUM;

		if (attachementType == AttachementType::Color) {
			texInternalFormat = TextureFormatToGLEnum(internalFormat);
			texFormat = TextureFormatToGLEnum(format);
		}
		else {
			texFormat = GL_DEPTH_COMPONENT;
			texInternalFormat = GL_DEPTH_COMPONENT;
		}

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, texInternalFormat, renderTarget.Width, renderTarget.Height, 0, texFormat, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (attachementType == AttachementType::Color) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderTarget.ColorAttachements.size(), GL_TEXTURE_2D, tex, 0);
			renderTarget.ColorAttachements.push_back(GLColorAttachement { .Id = tex, .InternalFormat = texInternalFormat, .Format = texFormat, .Type = GL_TEXTURE_2D});
		}
		else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
			renderTarget.DepthAttachement = GLDepthAttachement{ .Id = tex, .Type = GL_TEXTURE_2D };
		}

	}

	void OpenGL4API::RenderTargetAddBufferAttachment(HRenderTarget hRenderTarget, TextureFormat format, AttachementType attachementType) {
		GLRenderTarget& renderTarget = m_RenderTargets[(int)hRenderTarget];
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);

		GLenum bufferFormat = TextureFormatToGLEnum(format);

		GLuint buffer;
		glGenRenderbuffers(1, &buffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, bufferFormat, renderTarget.Width, renderTarget.Height);

		if (attachementType == AttachementType::Color) {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + renderTarget.ColorAttachements.size(), GL_RENDERBUFFER, buffer);
			renderTarget.ColorAttachements.push_back(GLColorAttachement{ .Id = buffer, .InternalFormat = bufferFormat, .Format = bufferFormat, .Type = GL_RENDERBUFFER });
		}
		else {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer);
			renderTarget.DepthAttachement = GLDepthAttachement{ .Id = buffer, .Type = GL_RENDERBUFFER };
		}
	}

	void OpenGL4API::FinalizeRenderTarget(HRenderTarget hRenderTarget) {
		GLRenderTarget renderTarget = m_RenderTargets[hRenderTarget];
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);

		GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		if (renderTarget.ColorAttachements.size() > 0) {
			glDrawBuffers(renderTarget.ColorAttachements.size(), attachments);
		}
		else {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status == GL_FRAMEBUFFER_COMPLETE) {
			return;
		}

		ZC_ERROR("Could not create framebuffer: ");
		switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED: ZC_FATAL_ERROR("GL_FRAMEBUFFER_UNDEFINED"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
		case GL_FRAMEBUFFER_UNSUPPORTED: ZC_FATAL_ERROR("GL_FRAMEBUFFER_UNSUPPORTED"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
		}
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
	void OpenGL4API::PushRenderCommand(const RenderCommand& command) {
		m_BatchedDrawing.Commands.push_back(command);
		m_BatchedDrawing.Invalidated = true;
	}

	void OpenGL4API::Flush() {
		// 1. Update Render Commands
		if (m_BatchedDrawing.Invalidated) {
			BufferData(m_BatchedDrawing.CommandBuffer, m_BatchedDrawing.Commands.size() * sizeof(RenderCommand), 0, m_BatchedDrawing.Commands.data());
			m_BatchedDrawing.Invalidated = false;
		}
	}

	void OpenGL4API::DrawArrays(DrawMode mode, uint32_t offset, uint32_t count) {
		GLenum glMode = DrawModeToGLEnum(mode);
		glDrawArrays(glMode, offset, count);
	}

	void OpenGL4API::MultiDrawArrays(DrawMode mode) {
		GLenum glMode = DrawModeToGLEnum(mode);
		glMultiDrawArraysIndirect(glMode, 0, m_BatchedDrawing.Commands.size(), 0);
	}

	#pragma endregion
}