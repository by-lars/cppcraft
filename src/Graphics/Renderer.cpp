#include "Graphics/Renderer.h"
#include "Core/Base.h"
#include "Platform/OpenGL/GL.h"
#include "Core/Game.h"
#include "Graphics/Vertex.h"
#include "Graphics/RenderAPI.h"
#include "Utility/Asset.h"
#include "Utility/Convert.h"
#include "Graphics/VertexPool.h"
#include "Data/HandleStore.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <queue>

namespace ZuneCraft {
	static float LineCubeVertices[] = {
		   0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f,
		   0.0f, 0.0f, 1.0f,
		   0.0f, 0.0f, 1.0f,
		   0.0f, 0.0f, 0.0f,

		   0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
		   0.0f, 1.0f, 1.0f,
		   0.0f, 1.0f, 1.0f,
		   0.0f, 1.0f, 0.0f,

		   1.0f, 0.0f, 0.0f,
		   1.0f, 1.0f, 0.0f,
		   0.0f, 0.0f, 0.0f,
		   0.0f, 1.0f, 0.0f,
		   1.0f, 0.0f, 1.0f,
		   1.0f, 1.0f, 1.0f,
		   0.0f, 0.0f, 1.0f,
		   0.0f, 1.0f, 1.0f,
	};

#ifdef ZC_PLATFORM_ZUNE
	//Texture coords are rotated, to render in landscape on a portrait buffer
	static const unsigned char FullscreenQuad[] = {
		0, 1, 0, 0,
		0, 0, 1, 0,
		1, 1, 0, 1,
		1, 0, 1, 1
	};
#elif ZC_PLATFORM_WIN32
	static const unsigned char FullscreenQuad[] = {
		0, 1, 0, 1,
		0, 0, 0, 0,
		1, 1, 1, 1,
		1, 0, 1, 0
	};
#endif


	static const size_t MAX_BATCH_MESHES = 512;

	Renderer::Renderer() {
		ZC_DEBUG("Initializing Renderer");

		m_RenderWidth = Game::Get().GetWindow().GetWidth();
		m_RenderHeight = Game::Get().GetWindow().GetHeight();
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)m_RenderWidth / (float)m_RenderHeight, 0.1f, 800.0f);

		//Initialize Rendering Device
		m_Device = RenderAPI::Create();

		//Setup default state
		m_Device->SetClearColor(0.51f, 0.64f, 1.0f, 1.0f);

		//Load Texturs
		Image atlas; Asset::GetImage("atlas.png", &atlas);
		m_Device->ActivateTextureSlot(1);
		m_Device->CreateTexture(atlas.Width, atlas.Height, atlas.GetFormat(), FilterMode::NEAREST)->Upload(atlas.Data);
	
		m_FXPipeline = m_Device->CreatePipeline()
			.SetViewport(0, 0, m_RenderWidth, m_RenderHeight)
			.SetDrawMode(DrawMode::TRIANGLE_STRIP)
			.AddShader("PostProcess")
			.AddStorage(StorageUsage::MESH_STATIC, StorageFormat::UBYTE_VEC4, 4, (void*)&FullscreenQuad[0])
			.Build();
		m_FXPipeline->PushCommand(RenderCommand{4,0,0,0});

		m_Device->ActivateTextureSlot(0);
		m_MeshPipeline = m_Device->CreatePipeline()
			.SetViewport(0, 0, m_RenderWidth, m_RenderHeight)
			.SetDrawMode(DrawMode::TRIANGLES)
			.AddShader("main")
			.AddStorage(StorageUsage::MESH_DYNAMIC, StorageFormat::UBYTE_VEC4_VEC4, MAX_BATCH_MESHES * (Chunk::VOLUME_SIZE / 8), nullptr)
			.AddStorage(StorageUsage::INSTANCE_DATA, StorageFormat::FLOAT_VEC3, MAX_BATCH_MESHES, nullptr)
			.AddRenderTexture(TextureFormat::RGBA)
			.AddRenderBuffer(TextureFormat::DEPTH_COMPONENT24)
			.Build();

		m_MeshPipeline->GetShader()->Set("uProj", proj);

		m_MeshPipeline->GetShader()->Set("uAtlas", 1);
		m_MeshPipeline->GetShader()->Set("uSkyColor", glm::vec3(0.51f, 0.64f, 1.0f));
		m_FXPipeline->GetShader()->Set("uTexture", 0);

		m_BatchMesh = new VertexPool(m_MeshPipeline, m_MeshPipeline->GetStorage(0));
		m_BatchDataBuffer = m_MeshPipeline->GetStorage(1);
		m_BatchCurrentOffset = 0;

		m_DebugStorage = m_Device->CreateStorage(StorageUsage::MESH_STATIC, StorageFormat::FLOAT_VEC3, 24, LineCubeVertices);
		m_DebugLayout = m_Device->CreateVertexLayout();
		m_DebugLayout->AddFormat(m_DebugStorage, StorageFormat::FLOAT_VEC3, StorageUsage::MESH_STATIC);
		m_DebugShader = m_Device->CreateShader("DebugShader");
		m_DebugShader->Set("uProj", proj);

		SetFlip(false);
	}

	Renderer::~Renderer() {
		delete m_BatchMesh;
	}

	void Renderer::DebugDrawCube(glm::vec3 pos, glm::vec3 color) {
		m_DebugCubePositions.push_back(DebugDrawData{pos, glm::vec3(1.0f), color});
	}

	void Renderer::DebugDrawCube(glm::vec3 pos, glm::vec3 size, glm::vec3 color) {
		m_DebugCubePositions.push_back(DebugDrawData{ pos, size, color });
	}


	void Renderer::SetView(const glm::mat4& viewMat) {
		m_MeshPipeline->GetShader()->Set("uView", viewMat);
		m_DebugShader->Set("uView", viewMat);
	}

	void Renderer::SetResolution(int width, int height) {

	}

	void Renderer::SetFlip(bool flipped) {
		m_FXPipeline->GetShader()->Set("uFlip", flipped == true ? -1 : 1);
	}

	void Renderer::Draw() {
		m_Device->BeginFrame();

		m_BatchMesh->Cleanup();
		m_BatchMesh->GenerateRenderCommands();
		m_MeshPipeline->Draw();
		
		m_DebugLayout->Bind();
		for (const auto& x : m_DebugCubePositions) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, x.Pos);
			model = glm::scale(model, x.Size);
			m_DebugShader->Set("uModel", model);
			m_DebugShader->Set("uColor", x.Color);
			m_Device->DrawArrays(DrawMode::LINES, m_DebugCubePositions.size() * 24);
		}
		
		m_FXPipeline->Draw();

		m_Device->EndFrame();

		m_DebugCubePositions.clear();
	}

	Id Renderer::BatchSubmitMesh(std::vector<Vertex>& mesh, const glm::vec3& translation) {
		BatchData data{};
		data.Translation = translation;

		size_t batchDataLocation = 0;

		//THIS needs improvement. Add Push and Erase to GPUStorage
		if (m_BatchDataFreeSlots.empty()) {
			m_BatchData.push_back(data);
			batchDataLocation = m_BatchData.size() - 1;
		}
		else {
			batchDataLocation = m_BatchDataFreeSlots.front();
			m_BatchData[batchDataLocation] = data;
			m_BatchDataFreeSlots.pop();
		}

		//ZC_ASSERT(batchDataLocation < UINT8_MAX, "Invalid batch data location");

		m_BatchDataBuffer->Upload((uint32_t)m_BatchData.size(), 0, &m_BatchData[0]);
	
		for (size_t i = 0; i < mesh.size(); i++) {
			mesh[i].BatchIndex = batchDataLocation;
		}

		VPNode* memLocation = m_BatchMesh->PushBack(&mesh[0], mesh.size(), batchDataLocation);
		m_BatchCurrentOffset += mesh.size();

		MeshInfo info { };
		info.MemoryLocation = memLocation;
		info.BatchId = batchDataLocation;
		return m_Meshes.PushBack(info);
	}

	void Renderer::BatchDrawMesh(Id hMesh) {
		//TODO: Use this to hide / show a mesh, used for frustum culling
	}

	void Renderer::BatchFreeMesh(Id hMesh) {
		MeshInfo& info = m_Meshes.Get(hMesh);
		m_BatchMesh->Free(info.MemoryLocation);
		m_Meshes.Erase(hMesh);
		m_BatchDataFreeSlots.push(info.BatchId);
	}
}