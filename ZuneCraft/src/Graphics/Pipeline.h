#pragma once
#include "Core/Base.h"
#include "Graphics/DeviceResources.h"

namespace ZuneCraft {
	class RenderAPI;
	class Pipeline {
	public:
		Pipeline(RenderAPI* device);
		~Pipeline() { }

		Pipeline& SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		Pipeline& SetDrawMode(DrawMode mode);
		Pipeline& SetShader(const std::string& name);
		Pipeline& AddStorage(StorageUsage usage, StorageFormat format, uint32_t initialSize, void* initialData);
		Pipeline& AddRenderTexture(TextureFormat format);
		Pipeline& AddRenderBuffer(TextureFormat format);
		Pipeline* Build();

		void Draw();
		void PushCommand(const RenderCommand& cmd);
		void ClearCommands();

		Shader* GetShader();
		GPUStorage* GetStorage(size_t index);

	private:
		struct { uint32_t X, Y, Width, Height; } m_Viewport;
		std::vector<GPUStorage*> m_Storages;
		DrawMode m_DrawMode;
		UsageHint m_UsageHint;
		RenderAPI* m_Device;
		Shader* m_Shader;
		RenderTarget* m_RenderTarget;
		CommandQueue* m_CommandQueue;
		VertexLayout* m_VertexLayout;
	};
}