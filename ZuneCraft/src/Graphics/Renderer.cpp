#include "Graphics/Renderer.h"
#include "Core/Base.h"
#include "Graphics/GL.h"
#include "Core/Game.h"
#include "Geometry/Vertex.h"
#include "Geometry/Mesher.h"
#include "Graphics/RenderAPI.h"
#include "Utility/Asset.h"
#include "Utility/Convert.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


#ifdef ZC_PLATFORM_ZUNE
#include <zdk.h> 
#endif

namespace ZuneCraft {
	static RenderAPI* s_Device = nullptr;

	static const size_t MAX_BATCH_MESHES = 128;

	struct BatchData {
		glm::vec3 Translation;
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
		Id BatchMeshBuffer;
		Id BatchDataBuffer;
		Id MeshShader;
		size_t BatchCurrentOffset;
		std::vector<BatchData> BatchData;
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
		BufferSpec meshBufferSpec = { };
		meshBufferSpec.Type = StorageType::VERTEX;
		meshBufferSpec.Usage = StorageUsage::DYNAMIC;
		meshBufferSpec.Format = StorageFormat::UBYTE_VEC4_VEC4;
		meshBufferSpec.ParrentBuffer = Handle::INVALID;
		meshBufferSpec.Count = (CHUNK_SIZE_QUBED/8) * MAX_BATCH_MESHES;
		s_Data.BatchMeshBuffer = s_Device->StorageCreate(meshBufferSpec);

		//Mesh Batch Data
		BufferSpec dataBufferSpec = { };
		dataBufferSpec.Type = StorageType::BATCH;
		dataBufferSpec.Usage = StorageUsage::DYNAMIC;
		dataBufferSpec.Format = StorageFormat::FLOAT_VEC3;
		dataBufferSpec.ParrentBuffer = s_Data.BatchMeshBuffer;
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
		s_Device->StorageUpload(s_Data.PPQuadBuffer, 4, 0, (void*)&Mesher::FullscreenQuad[0]);
	}	

	void Renderer::Shutdown() {
		delete s_Device;

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
		 
		s_Device->RenderTargetBind(s_Data.PPRenderTarget);
		s_Device->SetViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);
		s_Device->Clear();
	}

	void Renderer::EndFrame() {
		s_Device->Flush();

		s_Device->Draw(s_Data.MeshShader, s_Data.BatchMeshBuffer, DrawMode::TRIANGLES);

		s_Device->RenderTargetBind(Handle::INVALID);

#ifdef ZC_PLATFORM_ZUNE
		s_Device->SetViewport(0, 0, s_Data.RenderHeight, s_Data.RenderWidth);
#else
		s_Device->SetViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);
#endif
		s_Device->Clear();
		s_Device->DrawArrays(s_Data.PPShader, s_Data.PPQuadBuffer, DrawMode::TRIANGLE_STRIP, 0, 4);

		#ifdef ZC_PLATFORM_ZUNE
		ZDKGL_EndDraw();
		#endif
	}

	Id Renderer::BatchSubmitMesh(std::vector<Vertex>& mesh, const glm::vec3& translation) {
		BatchData data;
		data.Translation = translation;
		s_Data.BatchData.push_back(data);

		RenderCommand command;
		command.Count = mesh.size();
		command.InstanceCount = 1;
		command.First = s_Data.BatchCurrentOffset;
		command.BaseInstance = s_Data.BatchData.size() - 1;

		s_Device->RenderCommandPush(command);


		s_Device->StorageUpload(s_Data.BatchDataBuffer, s_Data.BatchData.size(), 0, &s_Data.BatchData[0]);
	
		for (int i = 0; i < mesh.size(); i++) {
			mesh[i].BatchIndex = s_Data.BatchData.size() - 1;
		}
	
		s_Device->StorageUpload(s_Data.BatchMeshBuffer, mesh.size(), s_Data.BatchCurrentOffset, &mesh[0]);
		s_Data.BatchCurrentOffset += mesh.size();

		return Handle::INVALID;
	}

	void Renderer::BatchDrawMesh(Id hMesh) {

	}

	void Renderer::BatchFreeMesh(Id hMesh) {

	}
}