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

		Pipeline& AddShader(const std::string& name);
		Pipeline& AddStorage(StorageUsage usage, StorageFormat format, uint32_t initialSize, void* initialData);
		
		Pipeline& AddRenderTexture(TextureFormat format);

		Pipeline& AddRenderBuffer(TextureFormat format);

		Pipeline* Build();

		void Draw();
		void PushCommand(const RenderCommand& cmd);
		void PushCommand(int count);

		void ClearCommands();

		Ref<Shader> GetShader();
		Ref<GPUStorage> GetStorage(size_t index);
		Ref<RenderTarget> GetRenderTarget();

	private:
		RenderAPI* m_Device;
		struct { uint32_t X, Y, Width, Height; } m_Viewport;
		std::vector<Ref<GPUStorage>> m_Storages;
		DrawMode m_DrawMode;
		UsageHint m_UsageHint;
		Ref<Shader> m_Shader;
		Ref<RenderTarget> m_RenderTarget;
		Ref<CommandQueue> m_CommandQueue;
		Ref<VertexLayout> m_VertexLayout;
		bool m_ClearOnDraw;
	};
}