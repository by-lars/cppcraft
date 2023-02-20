#include "Graphics/Pipeline.h"
#include "Graphics/RenderAPI.h"
#include <glm/glm.hpp>

namespace ZuneCraft {
	Pipeline::Pipeline(RenderAPI* device) {
		m_Device = device;
		m_Shader = nullptr;
		m_RenderTarget = nullptr;
		m_CommandQueue = nullptr;
		m_DrawMode = DrawMode::TRIANGLES;
		m_Viewport = { 0,0,0,0 };
		m_UsageHint = UsageHint::DYNAMIC;

		m_VertexLayout = m_Device->CreateVertexLayout();
	}

	Pipeline& Pipeline::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		m_Viewport = { x, y, width, height };
		return *this;
	}

	Pipeline& Pipeline::SetShader(const std::string& name) {
		m_Shader = m_Device->CreateShader(name);
		return *this;
	}

	Pipeline& Pipeline::SetDrawMode(DrawMode mode) {
		m_DrawMode = mode;
		return *this;
	}

	Pipeline& Pipeline::AddStorage(StorageUsage usage, StorageFormat format, uint32_t initialSize, void* initialData) {
		Ref<GPUStorage> storage = nullptr;

		if (usage == StorageUsage::INSTANCE_DATA && m_Device->GetCapabilities().AttributeDivisor == false) {
			ZC_ASSERT(m_Shader != nullptr, "API: " << m_Device->GetAPIName() << ": requires a Shader in order to add a INSTANCE_DATA buffer");
			ZC_ASSERT(initialData == nullptr, "API: " << m_Device->GetAPIName() << ": can't provide default data for INSTANCE_DATA buffer"); 

			storage = m_Device->CreateShaderStorage(m_Shader, "uBatchData", format, initialSize);
		}
		else {
			storage = m_Device->CreateStorage(usage, format, initialSize, initialData);
			m_VertexLayout->AddFormat(storage, format, usage);
		}
	
		m_Storages.push_back(storage);
		m_UsageHint = (UsageHint)(usage == StorageUsage::MESH_STATIC);
		return *this;
	}

	Pipeline& Pipeline::AddRenderTexture(TextureFormat format) {
		if (m_RenderTarget == nullptr) {
			m_RenderTarget = m_Device->CreateRenderTarget(m_Viewport.Width, m_Viewport.Height);
		}

		m_RenderTarget->AddTexture(format);

		return *this;
	}

	Pipeline& Pipeline::AddRenderBuffer(TextureFormat format) {
		if (m_RenderTarget == nullptr) {
			m_RenderTarget = m_Device->CreateRenderTarget(m_Viewport.Width, m_Viewport.Height);
		}

		m_RenderTarget->AddRenderBuffer(format);

		return *this;
	}

	Pipeline* Pipeline::Build() {
		if (m_RenderTarget == nullptr) {
			m_RenderTarget = m_Device->GetDefaultRenderTarget();
		}

		m_RenderTarget->Finalize();
		m_CommandQueue = m_Device->CreateCommandQueue(m_DrawMode, m_UsageHint);

		return this;
	}

	void Pipeline::Draw() {
		m_RenderTarget->Bind();
		m_Device->SetViewport(m_Viewport.X, m_Viewport.Y, m_Viewport.Width, m_Viewport.Height);
		m_Device->Clear();
		m_Shader->Bind();
		m_VertexLayout->Bind();
		m_CommandQueue->Draw();
	}

	void Pipeline::PushCommand(const RenderCommand& cmd) {
		ZC_ASSERT(m_CommandQueue != nullptr, "Command can only be pushed after build");
		m_CommandQueue->Push(cmd);
	}

	void Pipeline::ClearCommands() {
		ZC_ASSERT(m_CommandQueue != nullptr, "Commands can only be cleared after build");
		m_CommandQueue->Clear();
	}

	Ref<Shader> Pipeline::GetShader() {
		return m_Shader;
	}

	Ref<GPUStorage> Pipeline::GetStorage(size_t index) {
		return m_Storages[index];
	}
}