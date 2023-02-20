#pragma once
#include "Graphics/RenderAPI.h"
#include "Data/Handle.h"
#include "Data/HandleStore.h"
#include "Data/DenseArray.h"

#include <vector>

namespace ZuneCraft {
	class OpenGLAPI : public RenderAPI {
	public:
		OpenGLAPI();
		~OpenGLAPI();

		Pipeline& CreatePipeline() override;
		Ref<CommandQueue> CreateCommandQueue(DrawMode mode, UsageHint hint) override;
		Ref<VertexLayout> CreateVertexLayout() override;

		Ref<Shader>			CreateShader(const std::string& assetName) override;
		Ref<GPUStorage>		CreateStorage(StorageUsage usage, StorageFormat format, uint32_t initialCount, void* initialData) override;
		Ref<GPUStorage>		CreateShaderStorage(Ref<Shader>& shader, const std::string& location, StorageFormat format, uint32_t initialSize) override;
		Ref<Texture>		CreateTexture(uint32_t width, uint32_t height, TextureFormat format, FilterMode filterMode) override;
		Ref<RenderTarget>	CreateRenderTarget(GLuint width, GLuint height) override;

		Ref<RenderTarget> GetDefaultRenderTarget() override;
		
		void ActivateTextureSlot(uint32_t slot) override;

		void Clear() override;
		void SetClearColor(float r, float g, float b, float a) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void BeginFrame() override;
		void EndFrame() override;

		static GLenum TextureFormatToNative(TextureFormat format);

	private:
		Ref<RenderTarget> m_DefaultRenderTarget;
	};
}