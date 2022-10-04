#pragma once
#include "Graphics/DeviceResources.h"
#include "Graphics/Pipeline.h"
#include "Data/Handle.h"
#include "Utility/Asset.h"
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace ZuneCraft {
	class RenderAPI {
	public:
		ZC_ENUM API{
			NONE = 0,
			OPENGL_ES_2,
			OPENGL_4
		};

		struct Capabilities {
			bool AttributeDivisor;
		};

		static RenderAPI* Create();
		static RenderAPI::API GetAPI();
		static std::string GetAPIName();
		const Capabilities& GetCapabilities();

		virtual Pipeline& CreatePipeline() = 0;
		virtual CommandQueue* CreateCommandQueue(DrawMode drawMode, UsageHint hint) = 0;
		virtual VertexLayout* CreateVertexLayout() = 0;
		
		virtual Shader* CreateShader(const std::string& assetName) = 0;
		virtual GPUStorage* CreateStorage(StorageUsage usage, StorageFormat format, uint32_t initialCount, void* initialData) = 0;
		virtual GPUStorage* CreateShaderStorage(Shader* shader, const std::string& location, StorageFormat format, uint32_t initialSize) = 0;
		virtual Texture* CreateTexture(uint32_t width, uint32_t height, TextureFormat format, FilterMode filterMode) = 0;
		virtual RenderTarget* CreateRenderTarget(GLuint width, GLuint height) = 0;

		virtual RenderTarget* GetDefaultRenderTarget() = 0;

		virtual void ActivateTextureSlot(uint32_t slot) = 0;

		virtual void Clear() = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0; 

	protected:
		Capabilities m_Capabilities;
	};
}