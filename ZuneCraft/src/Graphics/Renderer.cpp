#include "Graphics/Renderer.h"
#include "Core/Base.h"
#include "Graphics/GL.h"
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
//
//#include <imgui/backends/imgui_impl_opengl3.h>
//#include <imgui/backends/imgui_impl_glfw.h>
//#include <imgui/imgui.h>

#ifdef ZC_PLATFORM_ZUNE
#include <zdk.h> 
#endif

namespace ZuneCraft {
	static const float LineCubeVertices[] = {
		   -0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
		   -0.5f, -0.5f, 0.5f,
		   -0.5f, -0.5f, 0.5f,
		   -0.5f, -0.5f, -0.5f,

		   -0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
		   -0.5f, 0.5f, 0.5f,
		   -0.5f, 0.5f, 0.5f,
		   -0.5f, 0.5f, -0.5f,

		   0.5f, -0.5f, -0.5f,
		   0.5f, 0.5f, -0.5f,
		   -0.5f, -0.5f, -0.5f,
		   -0.5f, 0.5f, -0.5f,
		   0.5f, -0.5f, 0.5f,
		   0.5f, 0.5f, 0.5f,
		   -0.5f, -0.5f, 0.5f,
		   -0.5f, 0.5f, 0.5f,
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


	static const size_t MAX_BATCH_MESHES = 128;

	Renderer::Renderer() {

	}

	Renderer::~Renderer() {

	}

	void Renderer::Init() {
		ZC_DEBUG("Initializing Renderer");
		m_RenderWidth = Game::Get().GetWindow().GetWidth();
		m_RenderHeight = Game::Get().GetWindow().GetHeight();

		//Initialize Rendering Device
		m_Device = RenderAPI::Create();

		//Setup default state
		m_Device->SetClearColor(0.51f, 0.64f, 1.0f, 1.0f);
		m_Device->SetViewport(0, 0, m_RenderWidth, m_RenderHeight);

		//Load Texture Atlas
		Image atlas; Asset::GetImage("atlas.png", &atlas);
		glActiveTexture(GL_TEXTURE1);
		TextureSpec atlasSpec = { };
		atlasSpec.Width = atlas.Width;
		atlasSpec.Height = atlas.Height;
		atlasSpec.Format = atlas.GetFormat();
		atlasSpec.DataType = DataType::UNSIGNED_BYTE;
		atlasSpec.ClampMode = ClampMode::CLAMP_TO_EDGE;
		atlasSpec.FilterMode = FilterMode::NEAREST;
		Id tex = m_Device->TextureCreate(atlasSpec);
		m_Device->TextureUploadData(tex, atlas.Data);

		//PP Frame Buffer
		glActiveTexture(GL_TEXTURE0);
		FramebufferSpec fboSpec = { };
		fboSpec.Width = m_RenderWidth;
		fboSpec.Height = m_RenderHeight;
		fboSpec.Attachements.push_back(FramebufferAttachement(TextureFormat::RGBA, AttachementType::Color, false));
		fboSpec.Attachements.push_back(FramebufferAttachement(TextureFormat::DEPTH_COMPONENT24, AttachementType::Depth, true));
		m_PPRenderTarget = m_Device->RenderTargetCreate(fboSpec);

		//Setup Post Processing Shader
		m_PPShader = m_Device->ShaderCreate("PostProcess");
		m_Device->SetShaderUniform(m_PPShader, "uTexture", 0);
		SetFlip(false);

		//Setup Default Mesh Shader Uniforms
		m_MeshShader = m_Device->ShaderCreate("main");
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)m_RenderWidth / (float)m_RenderHeight, 0.1f, 400.0f);
		m_Device->SetShaderUniform(m_MeshShader, "uProj", proj);
		m_Device->SetShaderUniform(m_MeshShader, "uAtlas", 1);
		m_Device->SetShaderUniform(m_MeshShader, "uSkyColor", glm::vec3(0.51f, 0.64f, 1.0f));

		//Mesh Buffer
		m_BatchMesh = new VertexPool<Vertex>(m_Device, StorageFormat::UBYTE_VEC4_VEC4, (Chunk::VOLUME_SIZE/8) * MAX_BATCH_MESHES);

		//Mesh Batch Data
		BufferSpec dataBufferSpec = { };
		dataBufferSpec.Type = StorageType::BATCH;
		dataBufferSpec.Usage = StorageUsage::DYNAMIC;
		dataBufferSpec.Format = StorageFormat::FLOAT_VEC3;
		dataBufferSpec.ParrentBuffer = m_BatchMesh->GetBufferId();
		dataBufferSpec.Count = MAX_BATCH_MESHES;
		dataBufferSpec.Shader = m_MeshShader;
		m_BatchDataBuffer = m_Device->StorageCreate(dataBufferSpec);
		m_BatchCurrentOffset = 0;

		//Setup Post Processing Fullscreen Quad
		BufferSpec quadBufferSpec = { };
		quadBufferSpec.Type = StorageType::VERTEX;
		quadBufferSpec.Usage = StorageUsage::STATIC;
		quadBufferSpec.Format = StorageFormat::UBYTE_VEC4;
		quadBufferSpec.Count = 4;
		quadBufferSpec.ParrentBuffer = Handle::INVALID;
		m_PPQuadBuffer = m_Device->StorageCreate(quadBufferSpec);
		m_Device->StorageUpload(m_PPQuadBuffer, 4, 0, (void*)&FullscreenQuad[0]);

		////Init ImGUI
		//IMGUI_CHECKVERSION();
		//ImGui::CreateContext();
		//ImGui::StyleColorsDark();
		//ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), true);
		//ImGui_ImplOpenGL3_Init("#version 130");
	}	

	void Renderer::Shutdown() {
		delete m_Device;
		//ImGui_ImplOpenGL3_Shutdown();
		//ImGui_ImplGlfw_Shutdown();
		//ImGui::DestroyContext();
	}

	void Renderer::SetView(const glm::mat4& viewMat) {
		m_Device->SetShaderUniform(m_MeshShader, "uView", viewMat);
	}

	void Renderer::SetResolution(int width, int height) {

	}

	void Renderer::SetFlip(bool flipped) {
		m_Device->SetShaderUniform(m_PPShader, "uFlip", flipped == true ? -1 : 1);
	}

	void Renderer::BeginFrame() {
		#ifdef ZC_PLATFORM_ZUNE
			ZDKGL_BeginDraw();
		#endif                  
		 

		m_BatchMesh->Cleanup();

		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		//ImGui::NewFrame();

		//m_BatchMesh->DebugDraw();

		m_Device->RenderTargetBind(m_PPRenderTarget);
		m_Device->SetViewport(0, 0, m_RenderWidth, m_RenderHeight);
		m_Device->Clear();
	}

	void Renderer::EndFrame() {
		m_Device->Flush();
		m_Device->Draw(m_MeshShader, m_BatchMesh->GetBufferId(), DrawMode::TRIANGLES);
		
		m_Device->RenderTargetBind(Handle::INVALID);
		
		#ifdef ZC_PLATFORM_ZUNE
		m_Device->SetViewport(0, 0, m_RenderHeight, m_RenderWidth);
		#else
		m_Device->SetViewport(0, 0, m_RenderWidth, m_RenderHeight);
		#endif

		m_Device->Clear();
		m_Device->DrawArrays(m_PPShader, m_PPQuadBuffer, DrawMode::TRIANGLE_STRIP, 0, 4);

		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		#ifdef ZC_PLATFORM_ZUNE
		ZDKGL_EndDraw();
		#endif
	}

	Id Renderer::BatchSubmitMesh(std::vector<Vertex>& mesh, const glm::vec3& translation) {
		BatchData data;
		data.Translation = translation;

		size_t batchDataLocation = 0;

		if (m_BatchDataFreeSlots.empty()) {
			m_BatchData.push_back(data);
			batchDataLocation = m_BatchData.size() - 1;
		}
		else {
			batchDataLocation = m_BatchDataFreeSlots.front();
			m_BatchData[batchDataLocation] = data;
			m_BatchDataFreeSlots.pop();
		}


		m_Device->StorageUpload(m_BatchDataBuffer, m_BatchData.size(), 0, &m_BatchData[0]);
	
		for (int i = 0; i < mesh.size(); i++) {
			mesh[i].BatchIndex = batchDataLocation;
		}

		VPNode* memLocation = m_BatchMesh->PushBack(mesh, batchDataLocation);
		m_BatchCurrentOffset += mesh.size();

		MeshInfo info;
		info.MemoryLocation = memLocation;
		info.BatchId = batchDataLocation;
		return m_Meshes.PushBack(info);
	}

	void Renderer::BatchDrawMesh(Id hMesh) {

	}

	void Renderer::BatchFreeMesh(Id hMesh) {
		MeshInfo& info = m_Meshes.Get(hMesh);
		m_BatchMesh->Free(info.MemoryLocation);
		m_Meshes.Erase(hMesh);
		m_BatchDataFreeSlots.push(info.BatchId);
	}
}