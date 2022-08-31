#include "Platform/OpenGL/OpenGL4API.h"
#include "Core/Base.h"
#include "Graphics/GL.h"

#include <glm/gtc/type_ptr.hpp>

namespace ZuneCraft {
	OpenGL4API::OpenGL4API() {
		ZC_LOG("Initializing OpenGL 4.6 API");

		const GLubyte* oglVersion = glGetString(GL_VERSION);
		const GLubyte* oglRenderer = glGetString(GL_RENDERER);
		const GLubyte* oglVendor = glGetString(GL_VENDOR);
		ZC_LOG("Running " << oglVersion << " on " << oglRenderer << " - " << oglVendor);

		m_BatchedDrawing.Invalidated = false;
		BufferSpec commandBufferSpec;
		commandBufferSpec.ParrentBuffer = Handle::INVALID;
		commandBufferSpec.Count = 128;
		commandBufferSpec.Usage = StorageUsage::DYNAMIC;
		commandBufferSpec.Type = StorageType::DRAW_COMMAND;
		m_BatchedDrawing.CommandBuffer = StorageCreate(commandBufferSpec);


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

	static void CompileShader(GLint shader, const GLchar* source) {
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint success = false;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == false) {
			char log[2048];
			glGetShaderInfoLog(shader, 2048, 0, log);
			ZC_FATAL_ERROR("Failed to compile shader: " << shader << "\n" << log);
		}
	}

	Id OpenGL4API::ShaderCreate(const std::string& assetName) {
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


	void OpenGL4API::SetShaderUniform(Id hShader, const std::string& name, const glm::vec3& value) {

		GLShader shader = m_Shaders[Handle::GetIndex(hShader)];
		glUseProgram(shader.Id);

		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGL4API::SetShaderUniform(Id hShader, const std::string& name, const glm::mat4& value) {

		GLShader shader = m_Shaders[Handle::GetIndex(hShader)];
		glUseProgram(shader.Id);

		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGL4API::SetShaderUniform(Id hShader, const std::string& name, int value) {

		GLShader shader = m_Shaders[Handle::GetIndex(hShader)];
		glUseProgram(shader.Id);
		int location = glGetUniformLocation(shader.Id, name.c_str());
		glUniform1i(location, value);
	}

	Id OpenGL4API::StorageCreate(BufferSpec& spec) {
		GLenum bufferType;
		switch (spec.Type) {
		case StorageType::VERTEX:
		case StorageType::BATCH:
			bufferType = GL_ARRAY_BUFFER;
			break;

		case StorageType::DRAW_COMMAND:
			bufferType = GL_DRAW_INDIRECT_BUFFER;
			break;

		default: ZC_FATAL_ERROR("Unknown buffer type enum"); break;
		}

		GLenum glUsage;
		switch (spec.Usage) {
		case ZuneCraft::StorageUsage::STATIC:	glUsage = GL_STATIC_DRAW;	break;
		case ZuneCraft::StorageUsage::DYNAMIC:	glUsage = GL_DYNAMIC_DRAW;	break;
		case ZuneCraft::StorageUsage::STREAM:	glUsage = GL_STREAM_DRAW;	break;
		default: ZC_FATAL_ERROR("Unknown buffer usage enum"); break;
		}


		GLuint glBufferId = 0;
		glGenBuffers(1, &glBufferId);
		glBindBuffer(bufferType, glBufferId);

		GLBuffer internalBuffer{
			.Id = glBufferId,
			.VertexArray = 0,
			.Type = bufferType,
			.AttributeCount = 0,
			.Stride = 0,
			.Count = spec.Count
		};


		if (spec.Type != StorageType::DRAW_COMMAND) {
			if (spec.ParrentBuffer == Handle::INVALID) {
				glGenVertexArrays(1, &internalBuffer.VertexArray);
				ZC_DEBUG_ALLOC("Created VAO ID=" << internalBuffer.VertexArray);
			}
			else {
				//Use VAO of another buffer, if one is provided
				GLBuffer parrentBuffer = m_Buffers[Handle::GetIndex(spec.ParrentBuffer)];
				internalBuffer.VertexArray = parrentBuffer.VertexArray;
				internalBuffer.AttributeCount = parrentBuffer.AttributeCount;

				ZC_ASSERT(parrentBuffer.AttributeCount > 0, "Parrent Buffer has to be setup first.");
				ZC_DEBUG("Attatching to VAO ID=" << internalBuffer.VertexArray << " from provided buffer");
			}

			glBindVertexArray(internalBuffer.VertexArray);

			int attrib = internalBuffer.AttributeCount;
			switch (spec.Format) {
			case StorageFormat::FLOAT_VEC3:
				internalBuffer.Stride = sizeof(GLfloat) * 3;
				glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, internalBuffer.Stride, (const void*)0);
				glEnableVertexAttribArray(attrib);
				internalBuffer.AttributeCount += 1;
				break;

			case StorageFormat::FLOAT_VEC4:
				internalBuffer.Stride = sizeof(GLfloat) * 4;
				glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, internalBuffer.Stride, (const void*)0);
				glEnableVertexAttribArray(attrib);
				internalBuffer.AttributeCount += 1;
				break;

			case StorageFormat::UBYTE_VEC4:
				internalBuffer.Stride = sizeof(GLubyte) * 4;
				glVertexAttribPointer(attrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, internalBuffer.Stride, (const void*)0);
				glEnableVertexAttribArray(attrib);
				internalBuffer.AttributeCount += 1;
				break;

			case StorageFormat::UBYTE_VEC4_VEC4:
				internalBuffer.Stride = sizeof(GLubyte) * 8;
				glVertexAttribPointer(attrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, internalBuffer.Stride, (const void*)0);
				glEnableVertexAttribArray(attrib);

				glVertexAttribPointer(attrib + 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, internalBuffer.Stride, (const void*)(4 * sizeof(GLubyte)));
				glEnableVertexAttribArray(attrib + 1);

				if (spec.Type == StorageType::BATCH) {
					glVertexAttribDivisor(attrib + 1, 1);
				}

				internalBuffer.AttributeCount += 2;
				break;
			default: ZC_FATAL_ERROR("Unknown Buffer Format"); break;
			}

			if (spec.Type == StorageType::BATCH) {
				glVertexAttribDivisor(attrib, 1); 
			}
		}
		else {
			internalBuffer.Stride = sizeof(RenderCommand);
		}

		glBufferData(bufferType, internalBuffer.Count * internalBuffer.Stride, 0, glUsage);

		ZC_DEBUG("Created ID=" << glBufferId << " of size=" << spec.Count * internalBuffer.Stride);


		m_Buffers.push_back(internalBuffer);
		return Handle::Create<GLBuffer>(m_Buffers.size() - 1);
	}

	void OpenGL4API::StorageUpload(Id hBuffer, uint32_t size, uint32_t offset, void* data) {
		ZC_ASSERT(Handle::IsOfType<GLBuffer>(hBuffer), "Invalid Handle");

		GLBuffer buffer = m_Buffers[Handle::GetIndex(hBuffer)];
		if (offset + size > buffer.Count) {
			ZC_WARN("Can't upload... buffer too small!");
			return;
		}
		glBindBuffer(buffer.Type, buffer.Id);
		glBufferSubData(buffer.Type, offset * buffer.Stride, size * buffer.Stride, data);
	}

	Id OpenGL4API::TextureCreate(const TextureSpec& spec) {
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

		m_Textures.push_back(GLTexture{
			.Id = textureId,
			.Width = spec.Width,
			.Height = spec.Height,
			.Format = glTextureFormat,
			.DataType = glDataType
			});

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, glTextureFormat, spec.Width, spec.Height, 0, glTextureFormat, glDataType, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glClampMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilterMode);

		return Handle::Create<GLTexture>(m_Textures.size() - 1);
	}

	void OpenGL4API::TextureBind(Id hTexture) {
		ZC_ASSERT(Handle::IsOfType<GLTexture>(hTexture), "Invalid Handle");

		GLTexture texture = m_Textures[Handle::GetIndex(hTexture)];
		glBindTexture(GL_TEXTURE_2D, texture.Id);
	}

	void OpenGL4API::TextureUploadData(Id hTexture, void* data) {
		ZC_ASSERT(Handle::IsOfType<GLTexture>(hTexture), "Invalid Handle");

		GLTexture texture = m_Textures[Handle::GetIndex(hTexture)];
		glBindTexture(GL_TEXTURE_2D, texture.Id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.Width, texture.Height, texture.Format, texture.DataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Id OpenGL4API::RenderTargetCreate(const FramebufferSpec& spec) {
		ZC_DEBUG("CreateRenderTarget");
		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		GLRenderTarget renderTarget;
		renderTarget.Id = fbo;
		renderTarget.Width = spec.Width;
		renderTarget.Height = spec.Height;

		int nrColorAttachements = 0;

		for (int i = 0; i < spec.Attachements.size(); i++) {
			const FramebufferAttachement& attachement = spec.Attachements[i];
			GLenum format = TextureFormatToGLEnum(attachement.Format);
			GLenum attachementPoint = GL_INVALID_ENUM;

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

				renderTarget.Attachements.push_back(GLAttachement{
					.Id = renderBuffer,
					.Format = format,
					.Type = GL_RENDERBUFFER
					});
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

				renderTarget.Attachements.push_back(GLAttachement{
					.Id = texture,
					.Format = format,
					.Type = GL_TEXTURE_2D
					});
			}
		}
		ZC_DEBUG("FinalizeRenderTarget");
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);

		GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		if (renderTarget.Attachements.size() > 0) {
			glDrawBuffers(nrColorAttachements, &attachments[0]);
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

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

		ZC_DEBUG("Finalized Render Target!");

		m_RenderTargets.push_back(renderTarget);
		return Handle::Create<GLRenderTarget>(m_RenderTargets.size() - 1);
	}

	void OpenGL4API::RenderTargetBind(Id hRenderTarget) {
		if (Handle::IsValid(hRenderTarget)) {
			ZC_ASSERT(Handle::IsOfType<GLRenderTarget>(hRenderTarget), "Invalid Handle");
			GLRenderTarget renderTarget = m_RenderTargets[Handle::GetIndex(hRenderTarget)];
			glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.Id);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	Id OpenGL4API::RenderCommandPush(const RenderCommand& command) {
		if (m_BatchedDrawing.Commands.Size() > 128) {
			ZC_WARN("Can't push more render commands");
			return Handle::INVALID;
		}
		Id handle = m_BatchedDrawing.Commands.PushBack(command);
		m_BatchedDrawing.Invalidated = true;
	
		return handle;
	}

	void OpenGL4API::RenderCommandErase(Id hRenderCommand) {
		m_BatchedDrawing.Commands.Erase(hRenderCommand);
		m_BatchedDrawing.Invalidated = true;
	}

	void OpenGL4API::Flush() {
		if (m_BatchedDrawing.Invalidated) {
			StorageUpload(m_BatchedDrawing.CommandBuffer, m_BatchedDrawing.Commands.Size(), 0, m_BatchedDrawing.Commands.Data());
			m_BatchedDrawing.Invalidated = false;
		}
	}

	void OpenGL4API::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGL4API::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void OpenGL4API::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGL4API::DrawArrays(Id hShader, Id hBuffer, DrawMode mode, uint32_t offset, uint32_t count) {
		ZC_ASSERT(Handle::IsOfType<GLShader>(hShader), "Invalid Handle");
		ZC_ASSERT(Handle::IsOfType<GLBuffer>(hBuffer), "Invalid Handle");

		glUseProgram(m_Shaders[Handle::GetIndex(hShader)].Id);
		glBindVertexArray(m_Buffers[Handle::GetIndex(hBuffer)].VertexArray);

		GLenum drawMode = DrawModeToGLEnum(mode);
		glDrawArrays(drawMode, offset, count);
	}

	void OpenGL4API::Draw(Id hShader, Id hBuffer, DrawMode mode) {
		ZC_ASSERT(Handle::IsOfType<GLShader>(hShader), "Invalid Handle");
		ZC_ASSERT(Handle::IsOfType<GLBuffer>(hBuffer), "Invalid Handle");

		glUseProgram(m_Shaders[Handle::GetIndex(hShader)].Id);
		glBindVertexArray(m_Buffers[Handle::GetIndex(hBuffer)].VertexArray);

		GLenum drawMode = DrawModeToGLEnum(mode);
		glMultiDrawArraysIndirect(drawMode, 0, m_BatchedDrawing.Commands.Size(), 0);
	}
}