#include "Core/Base.h"
#include "Core/Game.h"
#include "Platform/OpenGL/GLAPI.h"
#include "Platform/OpenGL/GL.h"

#include "Platform/OpenGL/GLTexture.h"
#include "Platform/OpenGL/GLRenderTarget.h"
#include "Platform/OpenGL/GLStorage.h"
#include "Platform/OpenGL/GLShader.h"
#include "Platform/OpenGL/GLVertexLayout.h"
#include "Platform/OpenGL/GLCommandQueue.h"

#include <glm/gtc/type_ptr.hpp>

namespace ZuneCraft {
	OpenGLAPI::OpenGLAPI() {
		ZC_DEBUG("Initializing RenderAPI::" << RenderAPI::GetAPIName());

		const GLubyte* oglVersion = glGetString(GL_VERSION);
		const GLubyte* oglRenderer = glGetString(GL_RENDERER);
		const GLubyte* oglVendor = glGetString(GL_VENDOR);
		ZC_LOG("Running " << oglVersion << " on " << oglRenderer << " - " << oglVendor);

		switch (GetAPI()) {
		case RenderAPI::API::OPENGL_4:
			m_Capabilities.AttributeDivisor = true;
			break;

		case RenderAPI::API::OPENGL_ES_2:
			m_Capabilities.AttributeDivisor = false;
			break;

		default:
			ZC_ASSERT(false, "Unknown OpenGL Version");
			break;
		}

		glEnable(GL_DEPTH_TEST);

		m_DefaultRenderTarget = CreateRef<GLRenderTarget>(0, Game::Get().GetWindow().GetWidth(), Game::Get().GetWindow().GetHeight());
	}

	OpenGLAPI::~OpenGLAPI() {
	
	}

	GLenum OpenGLAPI::TextureFormatToNative(TextureFormat format) {
		switch (format) {
			case TextureFormat::RGB: return GL_RGB; break;
			case TextureFormat::RGBA: return GL_RGBA; break;
			case TextureFormat::DEPTH_COMPONENT16: return GL_DEPTH_COMPONENT16; break;
	#ifdef ZC_PLATFORM_WIN32
			case TextureFormat::DEPTH_COMPONENT24: return GL_DEPTH_COMPONENT24; break;
			case TextureFormat::DEPTH_COMPONENT32: return GL_DEPTH_COMPONENT32; break;
	#endif
			default: ZC_ASSERT(false, "Unknown Texture Format"); break;
		}
	}

	Ref<Shader> OpenGLAPI::CreateShader(const std::string& assetName) {
		return CreateRef<GLShader>(assetName);
	}

	Pipeline& OpenGLAPI::CreatePipeline() {
		return *(new Pipeline(this));
	}

	Ref<CommandQueue> OpenGLAPI::CreateCommandQueue(DrawMode drawMode, UsageHint hint) {
		GLenum glDrawMode = GL_INVALID_ENUM;
		switch (drawMode) {
			case DrawMode::TRIANGLES: glDrawMode = GL_TRIANGLES; break;
			case DrawMode::TRIANGLE_STRIP: glDrawMode = GL_TRIANGLE_STRIP; break;
			case DrawMode::LINES: glDrawMode = GL_LINES; break;
			default: ZC_ASSERT(false, "Unknown Draw Mode"); break;
		}
		
		if (GetAPI() == RenderAPI::API::OPENGL_ES_2) {
			//GLES2 Does not support the "Draw Indirect Buffer"
			return CreateRef<GLDirectCommandQueue>(glDrawMode);
		}
		
		switch (hint) {
			case ZuneCraft::UsageHint::DYNAMIC: return CreateRef<GLIndirectCommandQueue>(this, glDrawMode); break;
			case ZuneCraft::UsageHint::STATIC: return CreateRef<GLDirectCommandQueue>(glDrawMode); break;
			default: ZC_ASSERT(false, "Unkown Usage Hint");break;
		}

		ZC_ASSERT(false, "Fatal Error - Did not retrun from switch");
	}

	Ref<VertexLayout> OpenGLAPI::CreateVertexLayout() {
		if (GetAPI() == RenderAPI::API::OPENGL_ES_2) {
			//GLES2 Does not support VertexArrayObjects
			return CreateRef<GLES2VertexLayout>();
		}

		return CreateRef<GLVertexLayout>();
	}

	Ref<GPUStorage> OpenGLAPI::CreateStorage(StorageUsage usage, StorageFormat format, uint32_t initialCount, void* initialData) {
		GLuint stride = 0;
		switch (format) {
			case StorageFormat::UBYTE_VEC4:		 stride = 4;					 break;
			case StorageFormat::UBYTE_VEC4_VEC4: stride = 8;					 break;
			case StorageFormat::FLOAT_VEC4:		 stride = sizeof(float) * 4; 	 break;
			case StorageFormat::FLOAT_VEC3:		 stride = sizeof(float) * 3;	 break;
			case StorageFormat::DRAW_COMMAND:	 stride = sizeof(RenderCommand); break;
			default: ZC_ASSERT(false, "Unknown StorageFormat Enum");			 break;
		}
		
	#ifdef ZC_PLATFORM_WIN32
		if (usage == StorageUsage::DRAW_COMMAND) {
			return CreateRef<GLVBOStorage>(GL_DRAW_INDIRECT_BUFFER, initialCount, stride, GL_DYNAMIC_DRAW, initialData);
		}
	#endif

		GLenum glUsage = (usage == StorageUsage::MESH_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
		return CreateRef<GLVBOStorage>(GL_ARRAY_BUFFER, initialCount, stride, glUsage, initialData);
	}

	Ref<GPUStorage> OpenGLAPI::CreateShaderStorage(Ref<Shader>& shader, const std::string& location, StorageFormat format, uint32_t initialSize) {
		return CreateRef<GLUniformStorage>(shader, location, format, initialSize);
	}

	Ref<Texture> OpenGLAPI::CreateTexture(uint32_t width, uint32_t height, TextureFormat format, FilterMode filterMode) {
		GLenum glTextureFormat = TextureFormatToNative(format);
		GLenum glMinFilterMode = GL_INVALID_ENUM;
		GLenum glMagFilterMode = GL_INVALID_ENUM;

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

		return CreateRef<GLTexture>(width, height, glTextureFormat, glMinFilterMode, glMagFilterMode);
	}


	void OpenGLAPI::ActivateTextureSlot(uint32_t slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
	}

	Ref<RenderTarget> OpenGLAPI::CreateRenderTarget(GLuint width, GLuint height) {
		return CreateRef<GLRenderTarget>(width, height);
	}

	void OpenGLAPI::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLAPI::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void OpenGLAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	Ref<RenderTarget> OpenGLAPI::GetDefaultRenderTarget() {
		return m_DefaultRenderTarget;
	}

	void OpenGLAPI::BeginFrame() {
	#ifdef ZC_PLATFORM_ZUNE
		ZDKGL_BeginDraw();
	#endif  
	}

	void OpenGLAPI::EndFrame() {

	}
}