#pragma once
#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include "Graphics/Pipeline.h"
#include "Data/Handle.h"
#include "Utility/Asset.h"
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

namespace ZuneCraft {
	class RenderAPI {
	public:
		enum class API{
			NONE = 0,
			OPENGL_ES_2,
			OPENGL_4
		};

		struct Capabilities {
			bool AttributeDivisor;
		};

		static std::shared_ptr<RenderAPI> Create();
		static RenderAPI::API GetAPI();
		static std::string GetAPIName();
		const Capabilities& GetCapabilities();

		virtual Pipeline& CreatePipeline() = 0;
		virtual Ref<CommandQueue> CreateCommandQueue(DrawMode drawMode, UsageHint hint) = 0;
		virtual Ref<VertexLayout> CreateVertexLayout() = 0;
		
		virtual Ref<Shader> CreateShader(const std::string& assetName) = 0;
		virtual Ref<GPUStorage> CreateStorage(StorageUsage usage, StorageFormat format, uint32_t initialCount, void* initialData) = 0;
		virtual Ref<GPUStorage> CreateShaderStorage(Ref<Shader>& shader, const std::string& location, StorageFormat format, uint32_t initialSize) = 0;
		virtual Ref<Texture> CreateTexture(uint32_t width, uint32_t height, TextureFormat format, FilterMode filterMode) = 0;
		virtual Ref<RenderTarget> CreateRenderTarget(GLuint width, GLuint height) = 0;
		virtual Ref<RenderTarget> GetDefaultRenderTarget() = 0;

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