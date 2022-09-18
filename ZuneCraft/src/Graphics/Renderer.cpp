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


	static RenderAPI* s_Device = nullptr;

	static const size_t MAX_BATCH_MESHES = 128;

	struct BatchData {
		glm::vec3 Translation;
	};

	struct MeshInfo {
		VPNode* MemoryLocation;
		size_t BatchId;
		Id RenderCommand;
	};

	struct RenderData {
		//General
		GLuint RenderWidth;
		GLuint RenderHeight;

		//PostProcessing
		Id PPShader;
		Id PPQuadBuffer;
		Id PPRenderTarget;

		//Mesh Rendering
		Id BatchDataBuffer;
		Id MeshShader;
		size_t BatchCurrentOffset;
		
		std::vector<BatchData> BatchData;
		VertexPool<Vertex>* BatchMesh;
		std::queue<size_t> BatchDataFreeSlots;

		HandleStore<MeshInfo> Meshes;
	};

	static RenderData s_Data;

	void Renderer::Init() {
		ZC_DEBUG("Initializing Renderer");
		s_Data.RenderWidth = Game::Get().GetWindow().GetWidth();
		s_Data.RenderHeight = Game::Get().GetWindow().GetHeight();

		//Initialize Rendering Device
		s_Device = RenderAPI::Create();

		//Setup default state
		s_Device->SetClearColor(0.51f, 0.64f, 1.0f, 1.0f);
		s_Device->SetViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);

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
		Id tex = s_Device->TextureCreate(atlasSpec);
		s_Device->TextureUploadData(tex, atlas.Data);

		//PP Frame Buffer
		glActiveTexture(GL_TEXTURE0);
		FramebufferSpec fboSpec = { };
		fboSpec.Width = s_Data.RenderWidth;
		fboSpec.Height = s_Data.RenderHeight;
		fboSpec.Attachements.push_back(FramebufferAttachement(TextureFormat::RGBA, AttachementType::Color, false));
		fboSpec.Attachements.push_back(FramebufferAttachement(TextureFormat::DEPTH_COMPONENT24, AttachementType::Depth, true));
		s_Data.PPRenderTarget = s_Device->RenderTargetCreate(fboSpec);

		//Setup Post Processing Shader
		s_Data.PPShader = s_Device->ShaderCreate("PostProcess");
		s_Device->SetShaderUniform(s_Data.PPShader, "uTexture", 0);
		SetFlip(false);

		//Setup Default Mesh Shader Uniforms
		s_Data.MeshShader = s_Device->ShaderCreate("main");
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)s_Data.RenderWidth / (float)s_Data.RenderHeight, 0.1f, 400.0f);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uProj", proj);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uAtlas", 1);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uSkyColor", glm::vec3(0.51f, 0.64f, 1.0f));

		//Mesh Buffer
		s_Data.BatchMesh = new VertexPool<Vertex>(s_Device, StorageFormat::UBYTE_VEC4_VEC4, (Chunk::VOLUME_SIZE/8) * MAX_BATCH_MESHES);

		//Mesh Batch Data
		BufferSpec dataBufferSpec = { };
		dataBufferSpec.Type = StorageType::BATCH;
		dataBufferSpec.Usage = StorageUsage::DYNAMIC;
		dataBufferSpec.Format = StorageFormat::FLOAT_VEC3;
		dataBufferSpec.ParrentBuffer = s_Data.BatchMesh->GetBufferId();
		dataBufferSpec.Count = MAX_BATCH_MESHES;
		dataBufferSpec.Shader = s_Data.MeshShader;
		s_Data.BatchDataBuffer = s_Device->StorageCreate(dataBufferSpec);
		s_Data.BatchCurrentOffset = 0;

		//Setup Post Processing Fullscreen Quad
		BufferSpec quadBufferSpec = { };
		quadBufferSpec.Type = StorageType::VERTEX;
		quadBufferSpec.Usage = StorageUsage::STATIC;
		quadBufferSpec.Format = StorageFormat::UBYTE_VEC4;
		quadBufferSpec.Count = 4;
		quadBufferSpec.ParrentBuffer = Handle::INVALID;
		s_Data.PPQuadBuffer = s_Device->StorageCreate(quadBufferSpec);
		s_Device->StorageUpload(s_Data.PPQuadBuffer, 4, 0, (void*)&FullscreenQuad[0]);

		////Init ImGUI
		//IMGUI_CHECKVERSION();
		//ImGui::CreateContext();
		//ImGui::StyleColorsDark();
		//ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Game::Get().GetWindow().GetNativeWindow(), true);
		//ImGui_ImplOpenGL3_Init("#version 130");
	}	

	void Renderer::Shutdown() {
		delete s_Device;
		//ImGui_ImplOpenGL3_Shutdown();
		//ImGui_ImplGlfw_Shutdown();
		//ImGui::DestroyContext();
	}

	void Renderer::SetView(const glm::mat4& viewMat) {
		s_Device->SetShaderUniform(s_Data.MeshShader, "uView", viewMat);
	}

	void Renderer::SetResolution(int width, int height) {

	}

	void Renderer::SetFlip(bool flipped) {
		s_Device->SetShaderUniform(s_Data.PPShader, "uFlip", flipped == true ? -1 : 1);
	}

	void Renderer::BeginFrame() {
		#ifdef ZC_PLATFORM_ZUNE
			ZDKGL_BeginDraw();
		#endif                  
		 

		s_Data.BatchMesh->Cleanup();

		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		//ImGui::NewFrame();

		//s_Data.BatchMesh->DebugDraw();

		s_Device->RenderTargetBind(s_Data.PPRenderTarget);
		s_Device->SetViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);
		s_Device->Clear();
	}

	void Renderer::EndFrame() {
		s_Device->Flush();
		s_Device->Draw(s_Data.MeshShader, s_Data.BatchMesh->GetBufferId(), DrawMode::TRIANGLES);
		
		s_Device->RenderTargetBind(Handle::INVALID);
		
		#ifdef ZC_PLATFORM_ZUNE
		s_Device->SetViewport(0, 0, s_Data.RenderHeight, s_Data.RenderWidth);
		#else
		s_Device->SetViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);
		#endif

		s_Device->Clear();
		s_Device->DrawArrays(s_Data.PPShader, s_Data.PPQuadBuffer, DrawMode::TRIANGLE_STRIP, 0, 4);

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

		if (s_Data.BatchDataFreeSlots.empty()) {
			s_Data.BatchData.push_back(data);
			batchDataLocation = s_Data.BatchData.size() - 1;
		}
		else {
			batchDataLocation = s_Data.BatchDataFreeSlots.front();
			s_Data.BatchData[batchDataLocation] = data;
			s_Data.BatchDataFreeSlots.pop();
		}


		s_Device->StorageUpload(s_Data.BatchDataBuffer, s_Data.BatchData.size(), 0, &s_Data.BatchData[0]);
	
		for (int i = 0; i < mesh.size(); i++) {
			mesh[i].BatchIndex = batchDataLocation;
		}

		VPNode* memLocation = s_Data.BatchMesh->PushBack(mesh, batchDataLocation);
		s_Data.BatchCurrentOffset += mesh.size();

		MeshInfo info;
		info.MemoryLocation = memLocation;
		info.BatchId = batchDataLocation;
		return s_Data.Meshes.PushBack(info);
	}

	void Renderer::BatchDrawMesh(Id hMesh) {

	}

	void Renderer::BatchFreeMesh(Id hMesh) {
		MeshInfo& info = s_Data.Meshes.Get(hMesh);
		s_Data.BatchMesh->Free(info.MemoryLocation);
		s_Data.Meshes.Erase(hMesh);
		s_Data.BatchDataFreeSlots.push(info.BatchId);
	}
}