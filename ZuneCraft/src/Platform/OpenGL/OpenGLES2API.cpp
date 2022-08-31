#include "Core/Base.h"
#include "Platform/OpenGL/OpenGLES2API.h"
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/GL.h"
#include <vector>
#include <windows.h>

namespace ZuneCraft {

#ifdef ZC_PLATFORM_ZUNE
	PFNGLDRAWBUFFERSARBPROC glDrawBuffers;
	PFNGLMULTIDRAWARRAYSEXTPROC glMultiDrawArrays;
#endif

#pragma region Constructor
	OpenGLES2API::OpenGLES2API() {
		ZC_LOG("Initializing OpenGLES 2.0 API");

		std::string oglVersion((const char*)glGetString(GL_VERSION));
		std::string oglRenderer((const char*)glGetString(GL_RENDERER));
		std::string oglVendor((const char*)glGetString(GL_VENDOR));

		ZC_LOG("Running " << oglVersion << " on " << oglRenderer << " - " << oglVendor);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		//Load Extensions
#ifdef ZC_PLATFORM_ZUNE
		glDrawBuffers = NULL;
		glMultiDrawArrays = NULL;

		glDrawBuffers = (PFNGLDRAWBUFFERSARBPROC)eglGetProcAddress("glDrawBuffersARB");
		ZC_ASSERT(glDrawBuffers != NULL, "Could not load glDrawBuffersARB");
		
		//glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSEXTPROC)eglGetProcAddress("glMultiDrawArraysEXT");
		//ZC_ASSERT(glMultiDrawArrays != NULL, "Could not load glMultiDrawArraysEXT");
#endif
	}

	OpenGLES2API::~OpenGLES2API() {
		for (int i = 0; i < m_Shaders.size(); i++) {
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
		case TextureFormat::DEPTH_COMPONENT32:
		case TextureFormat::DEPTH_COMPONENT24:
		case TextureFormat::DEPTH_COMPONENT16: return GL_DEPTH_COMPONENT16; break;
		default: ZC_FATAL_ERROR("Unkown Texture Format"); break;
		}
	}

	static void CompileShader(GLint shader, const GLchar* source) {
#ifdef ZC_PLATFORM_WIN32
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint success = false;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == false) {
			char log[2048];
			glGetShaderInfoLog(shader, 2048, 0, log);
			ZC_FATAL_ERROR("Failed to compile shader: " << shader << "\n" << log);
		}
#endif
	}

	Id OpenGLES2API::ShaderFromSource(const std::string& assetName) {
		std::string vertexCode = Asset::GetShaderSource(assetName + ".vs");
		std::string fragmentCode = Asset::GetShaderSource(assetName + ".fs");

		GLuint program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		ZC_DEBUG("Compiling Shader: " << " Id= " << program);

		CompileShader(vertex, vertexCode.c_str());
		CompileShader(fragment, fragmentCode.c_str());

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);

		std::vector<std::string> attributes;
		Asset::GetShaderAttribs(assetName + ".attribs", &attributes);

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

		return Handle::Create<GLShader>(m_Shaders.size() - 1);
	}

	Id OpenGLES2API::ShaderFromBinary(const std::string& assetName) {
		std::vector<char> vertexBinary;
		Asset::GetShaderBinary(assetName + ".nvbv", &vertexBinary);

		std::vector<char> fragmentBinary;
		Asset::GetShaderBinary(assetName + ".nvbf", &fragmentBinary);

		GLuint program = glCreateProgram();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderBinary(1, &vertexShader, GL_NVIDIA_PLATFORM_BINARY_NV, &vertexBinary[0], vertexBinary.size());
		glShaderBinary(1, &fragmentShader, GL_NVIDIA_PLATFORM_BINARY_NV, &fragmentBinary[0], fragmentBinary.size());

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		std::vector<std::string> attributes;
		Asset::GetShaderAttribs(assetName + ".attribs", &attributes);

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

		return Handle::Create<GLShader>(m_Shaders.size() - 1);
	}

	Id OpenGLES2API::ShaderCreate(const std::string& assetName) {
#ifdef ZC_PLATFORM_ZUNE
		return ShaderFromBinary(assetName);
#else
		return ShaderFromSource(assetName);
#endif
	}

	void OpenGLES2API::SetShaderUniform(Id hShader, const std::string& name, const glm::vec3& value) {

		GLShader shader = m_Shaders[Handle::GetIndex(hShader)];
		glUseProgram(shader.Id);

		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLES2API::SetShaderUniform(Id hShader, const std::string& name, const glm::mat4& value) {

		GLShader shader = m_Shaders[Handle::GetIndex(hShader)];
		glUseProgram(shader.Id);

		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLES2API::SetShaderUniform(Id hShader, const std::string& name, int value) {

		GLShader shader = m_Shaders[Handle::GetIndex(hShader)];
		glUseProgram(shader.Id);
		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniform1i(location, value);
	}

	Id OpenGLES2API::CreateShaderUniform(const BufferSpec& spec, const std::string& name) {
		GLShader shader = m_Shaders[Handle::GetIndex(spec.Shader)];
		glUseProgram(shader.Id);

		GLint location = glGetUniformLocation(shader.Id, name.c_str());
		ZC_DEBUG("Creating Shader Storage location=" << location);

		GLUniform uniform;
		uniform.Name = name;
		uniform.Location = location;
		uniform.ProgramId = shader.Id;
		uniform.Count = spec.Count;
		uniform.Format = spec.Format;

		m_ShaderUniforms.push_back(uniform);

		return Handle::Create<GLUniform>(m_ShaderUniforms.size() - 1);
	}

	Id OpenGLES2API::CreateBuffer(const BufferSpec& spec) {
		ZC_ASSERT(Handle::IsValid(spec.ParrentBuffer) == false, "VAOs are not supported by GLES2");

		GLES2Buffer internalBuffer = { };
		GLES2BufferElement e = { };
		e.Offset = 0;

		switch (spec.Format) {
		case StorageFormat::UBYTE_VEC4:
			e.Count = 4;
			e.DataType = GL_UNSIGNED_BYTE;
			internalBuffer.VertexLayout.push_back(e);
			internalBuffer.Stride = sizeof(GLubyte) * 4;
			break;
		case StorageFormat::UBYTE_VEC4_VEC4:
			e.Count = 4;
			e.DataType = GL_UNSIGNED_BYTE;
			internalBuffer.VertexLayout.push_back(e);
			e.Offset = 4;
			internalBuffer.VertexLayout.push_back(e);
			internalBuffer.Stride = sizeof(GLubyte) * 8;
			break;

		case StorageFormat::FLOAT_VEC3:
			e.Count = 3;
			e.DataType = GL_FLOAT;
			internalBuffer.VertexLayout.push_back(e);
			internalBuffer.Stride = sizeof(GLfloat) * 3;
			break;

		case StorageFormat::FLOAT_VEC4:
			e.Count = 4;
			e.DataType = GL_FLOAT;
			internalBuffer.VertexLayout.push_back(e);
			internalBuffer.Stride = sizeof(GLfloat) * 4;
			break;

		case StorageFormat::INT_VEC1:
			e.Count = 1;
			e.DataType = GL_INT;
			internalBuffer.VertexLayout.push_back(e);
			internalBuffer.Stride = sizeof(GLint);
			break;

		default: ZC_FATAL_ERROR("Unknown Storage Format") break;
		}

		GLuint glBufferId = 0;
		glGenBuffers(1, &glBufferId);
		internalBuffer.Id = glBufferId;
		internalBuffer.Count = spec.Count;

		GLsizei sizeInBytes = internalBuffer.Stride * internalBuffer.Count;

		GLenum glUsage;
		switch (spec.Usage) {
		case ZuneCraft::StorageUsage::STATIC:	glUsage = GL_STATIC_DRAW;	break;
		case ZuneCraft::StorageUsage::DYNAMIC:	glUsage = GL_DYNAMIC_DRAW;	break;
		case ZuneCraft::StorageUsage::STREAM:	glUsage = GL_STREAM_DRAW;	break;
		default: ZC_FATAL_ERROR("Unknown buffer usage enum"); break;
		}

		glBindBuffer(GL_ARRAY_BUFFER, glBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, 0, glUsage);
		
		ZC_DEBUG("Created ID=" << glBufferId << " of size=" << sizeInBytes);

		m_Buffers.push_back(internalBuffer);
		return Handle::Create<GLES2Buffer>(m_Buffers.size() - 1);
	}

	Id OpenGLES2API::StorageCreate(BufferSpec& spec) {
		switch (spec.Type) {
		case StorageType::VERTEX:
			return CreateBuffer(spec);

		case StorageType::BATCH:
			return CreateShaderUniform(spec, "uBatchData[0]");
		case StorageType::SHADER:
			ZC_ASSERT(false, "Not Implemented"); break;
		//	return CreateShaderUniform(spec);

		default: ZC_FATAL_ERROR("Unknown Buffer Type");
		}
	}

	void OpenGLES2API::StorageUpload(Id hBuffer, uint32_t size, uint32_t offset, void* data) {
		ZC_ASSERT(Handle::IsOfType<GLES2Buffer>(hBuffer) || Handle::IsOfType<GLUniform>(hBuffer), "Invalid Handle");

		if (Handle::IsOfType<GLES2Buffer>(hBuffer)) {
			GLES2Buffer buffer = m_Buffers[Handle::GetIndex(hBuffer)];
			
			if (offset + size > buffer.Count) {
				ZC_WARN("Can't upload... buffer too small!");
				return;
			}

			glBindBuffer(GL_ARRAY_BUFFER, buffer.Id);
			glBufferSubData(GL_ARRAY_BUFFER, offset * buffer.Stride, size * buffer.Stride, data);
			ZC_DEBUG("Uploaded " << size * buffer.Stride << " bytes to buffer");
		}
		else if (Handle::IsOfType<GLUniform>(hBuffer)) {
			GLUniform uniform = m_ShaderUniforms[Handle::GetIndex(hBuffer)];
			GLint location = uniform.Location;
			glUseProgram(uniform.ProgramId);

			if (offset + size > uniform.Count) {
				ZC_WARN("Can't upload... uniform too small!");
				return;
			}

			if (offset != 0) {
				std::stringstream ss;
				ss << uniform.Name << "[" << offset << "]";
				std::string name = ss.str();
				location = glGetUniformLocation(uniform.ProgramId, name.c_str());
			}

			switch (uniform.Format) {
			case StorageFormat::FLOAT_VEC3: glUniform3fv(location, size, (const GLfloat*)data); break;
			case StorageFormat::FLOAT_VEC4: glUniform4fv(location, size, (const GLfloat*)data); break;
			case StorageFormat::INT_VEC1: glUniform1iv(location, size, (const GLint*)data); break;

			default: ZC_FATAL_ERROR("Unknown Uniform Format");
			}

			ZC_DEBUG("Uploaded " << size << " to uniform");
		}
	}

	void OpenGLES2API::BindBuffer(Id hBuffer) {
		ZC_ASSERT(Handle::IsOfType<GLES2Buffer>(hBuffer), "Invalid Handle");

		GLES2Buffer buffer = m_Buffers[Handle::GetIndex(hBuffer)];
		glBindBuffer(GL_ARRAY_BUFFER, buffer.Id);

		for (int i = 0; i < buffer.VertexLayout.size(); i++) {
			glVertexAttribPointer(i, buffer.VertexLayout[i].Count, buffer.VertexLayout[i].DataType, false, buffer.Stride, (const void*)buffer.VertexLayout[i].Offset);
			glEnableVertexAttribArray(i);
		}
	}

	Id OpenGLES2API::TextureCreate(const TextureSpec& spec) {
		GLuint textureId = 0;
		glGenTextures(1, &textureId);

		GLenum glDataType = DataTypeToGLEnum(spec.DataType);
		GLenum glTextureFormat = TextureFormatToGLEnum(spec.Format);
		GLenum glClampMode = GL_INVALID_ENUM;
		GLenum glMinFilterMode = GL_INVALID_ENUM;
		GLenum glMagFilterMode = GL_INVALID_ENUM;

		switch (spec.ClampMode) {
		case ClampMode::REPEAT: glClampMode = GL_REPEAT; break;
		case ClampMode::CLAMP_TO_EDGE: glClampMode = GL_CLAMP_TO_EDGE; break;
		default: ZC_FATAL_ERROR("Unkown Clamp Mode"); break;
		}

		switch (spec.FilterMode) {
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

		GLTexture texture;
		texture.Id = textureId;
		texture.Width = spec.Width;
		texture.Height = spec.Height;
		texture.Format = glTextureFormat;
		texture.DataType = glDataType;
		m_Textures.push_back(texture);


		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, glTextureFormat, spec.Width, spec.Height, 0, glTextureFormat, glDataType, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilterMode);

		return Handle::Create<GLTexture>(m_Textures.size() - 1);
	}

	void OpenGLES2API::TextureBind(Id hTexture) {
		ZC_ASSERT(Handle::IsOfType<GLTexture>(hTexture), "Invalid Handle");

		GLTexture texture = m_Textures[Handle::GetIndex(hTexture)];
		glBindTexture(GL_TEXTURE_2D, texture.Id);
	}

	void OpenGLES2API::TextureUploadData(Id hTexture, void* data) {
		ZC_ASSERT(Handle::IsOfType<GLTexture>(hTexture), "Invalid Handle");

		GLTexture texture = m_Textures[Handle::GetIndex(hTexture)];
		glBindTexture(GL_TEXTURE_2D, texture.Id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.Width, texture.Height, texture.Format, texture.DataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Id OpenGLES2API::RenderTargetCreate(const FramebufferSpec& spec) {
		ZC_DEBUG("CreateRenderTarget");
		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		GLRenderTarget renderTarget;
		renderTarget.Id = fbo;
		renderTarget.Width = spec.Width;
		renderTarget.Height = spec.Height;

		for (int i = 0; i < spec.Attachements.size(); i++) {
			const FramebufferAttachement& attachement = spec.Attachements[i];
			GLenum format = TextureFormatToGLEnum(attachement.Format);
			GLenum attachementPoint = GL_INVALID_ENUM;
			int nrColorAttachements = 0;

			switch (attachement.Type) {
			case AttachementType::Color:
				attachementPoint = GL_COLOR_ATTACHMENT0 + nrColorAttachements;
				nrColorAttachements++;
				break;
			case AttachementType::Depth:
				attachementPoint = GL_DEPTH_ATTACHMENT;
				break;
			default:
				ZC_FATAL_ERROR("Unknown Attachement Type");
				break;
			}

			if (attachement.WriteOnly) {
				//Create RenderBuffer if WriteOnly is requested
				GLuint renderBuffer;
				glGenRenderbuffers(1, &renderBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, format, renderTarget.Width, renderTarget.Height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachementPoint, GL_RENDERBUFFER, renderBuffer);

				GLAttachement a;
				a.Format = format;
				a.Id = renderBuffer;
				a.Type = GL_RENDERBUFFER;
				renderTarget.Attachements.push_back(a);
			}
			else {
				//Otherwise Create a texture
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, format, renderTarget.Width, renderTarget.Height, 0, format, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachementPoint, GL_TEXTURE_2D, texture, 0);

				GLAttachement a;
				a.Format = format;
				a.Id = texture;
				a.Type = GL_TEXTURE_2D;
				renderTarget.Attachements.push_back(a);
			}
		}
		ZC_DEBUG("FinalizeRenderTarget");
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);

		GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		if (renderTarget.Attachements.size() > 0) {
			//glDrawBuffers(renderTarget.ColorAttachements.size(), &attachments[0]);
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: ZC_FATAL_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_UNSUPPORTED: ZC_FATAL_ERROR("GL_FRAMEBUFFER_UNSUPPORTED"); break;
		}

		ZC_DEBUG("Finalized Render Target!");

		m_RenderTargets.push_back(renderTarget);
		return Handle::Create<GLRenderTarget>(m_RenderTargets.size() - 1);
	}

	void OpenGLES2API::RenderTargetBind(Id hRenderTarget) {
		if (Handle::IsValid(hRenderTarget)) {
			ZC_ASSERT(Handle::IsOfType<GLRenderTarget>(hRenderTarget), "Invalid Handle");
			GLRenderTarget renderTarget = m_RenderTargets[Handle::GetIndex(hRenderTarget)];
			glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	Id OpenGLES2API::RenderCommandPush(const RenderCommand& command) {
		return m_RenderCommands.PushBack(command);
	}

	void OpenGLES2API::RenderCommandErase(Id hRenderCommand) {
		m_RenderCommands.Erase(hRenderCommand);
	}

	void OpenGLES2API::Flush() {
		// Nothing todo on ES2
	}

	void OpenGLES2API::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLES2API::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void OpenGLES2API::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLES2API::DrawArrays(Id hShader, Id hBuffer, DrawMode mode, uint32_t offset, uint32_t count) {
		ZC_ASSERT(Handle::IsOfType<GLShader>(hShader), "Invalid Handle");
		ZC_ASSERT(Handle::IsOfType<GLES2Buffer>(hBuffer), "Invalid Handle");

		glUseProgram(m_Shaders[Handle::GetIndex(hShader)].Id);
		BindBuffer(hBuffer);
		GLenum drawMode = DrawModeToGLEnum(mode);
		glDrawArrays(drawMode, offset, count);
	}

	void OpenGLES2API::Draw(Id hShader, Id hBuffer, DrawMode mode) {
		glUseProgram(m_Shaders[Handle::GetIndex(hShader)].Id);
		BindBuffer(hBuffer);
		GLenum drawMode = DrawModeToGLEnum(mode);

		for (int i = 0; i < m_RenderCommands.Size(); i++) {
			glDrawArrays(drawMode, m_RenderCommands[i].First, m_RenderCommands[i].Count);
		}
	}
}