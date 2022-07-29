#include "Graphics/Renderer.h"
#include "Core/Base.h"
#include "Graphics/GL.h"
#include "Core/Game.h"

#include "Geometry/Vertex.h"
#include "Geometry/Mesher.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Graphics/RenderAPI.h"
#include "Utility/File.h"
#include "Utility/SparseArray.h"

#ifdef ZC_PLATFORM_ZUNE
#include <zdk.h> 
#endif

namespace ZuneCraft {
	static RenderAPI* s_Device = nullptr;

	static const size_t MAX_BATCH_MESHES = 500;

	struct BatchData {
		glm::mat4 Translation;
	};

	struct RenderData {
		//General
		GLuint RenderWidth;
		GLuint RenderHeight;

		//PostProcessing
		HShader PPShader;
		HBuffer PPQuadBuffer;

		//Mesh Rendering
		//SparseArray<PolyMesh, MAX_BATCH_MESHES> BatchedMeshes;
		HBuffer BatchMeshBuffer;
		HBuffer BatchDataBuffer;
		std::vector<BatchData> BatchData;
		size_t BatchCurrentOffset;
		HShader MeshShader;

		//Indirect Drawing
		HBuffer RenderCommandBuffer;
		std::vector<RenderCommand> RenderCommands;

	};

	static RenderData s_Data;

	void Renderer::Init() {
		ZC_DEBUG("Initializing Renderer");
		s_Data.RenderWidth = Game::Get().GetWindow().GetWidth();
		s_Data.RenderHeight = Game::Get().GetWindow().GetHeight();

		s_Device = RenderAPI::Create();
		RenderAPI::Capabilities apiCapabilities = s_Device->GetCapabilities();

		s_Device->SetClearColor(0.51f, 0.64f, 1.0f, 1.0f);
		s_Device->SetViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);

		//Setup Post Processing Shader
		std::vector<std::string> ppShaderAttribs; ppShaderAttribs.push_back("aPos");
		s_Data.PPShader = s_Device->CreateShader(
			File::LoadTextFile("shader\\GL46\\PostProcess.vs"), 
			File::LoadTextFile("shader\\GL46\\PostProcess.fs"), 
			ppShaderAttribs
		);
		s_Device->BindShader(s_Data.PPShader);
		s_Device->SetShaderUniform(s_Data.PPShader, "uTexture", 0);
		SetFlip(false);

		s_Data.PPQuadBuffer = s_Device->CreateBuffer(sizeof(Mesher::FullscreenQuad), BufferType::ARRAY, BufferUsage::STATIC_DRAW);
		s_Device->BindBuffer(s_Data.PPQuadBuffer);
		s_Device->BufferData(s_Data.PPQuadBuffer, sizeof(Mesher::FullscreenQuad), 0, (void*)&Mesher::FullscreenQuad[0]);
		s_Device->SetBufferLayout(s_Data.PPQuadBuffer, {
			{DataType::UNSIGNED_BYTE, 4, 0}
		});

		//Setup Indirect Rendering
		s_Data.RenderCommandBuffer = s_Device->CreateBuffer(sizeof(RenderCommand) * MAX_BATCH_MESHES, BufferType::DRAW_INDIRECT_BUFFER, BufferUsage::DYNAMIC_DRAW);
		
		s_Data.BatchMeshBuffer = s_Device->CreateBuffer(CHUNK_SIZE_QUBED * MAX_BATCH_MESHES, BufferType::ARRAY, BufferUsage::DYNAMIC_DRAW);
		s_Device->SetBufferLayout(s_Data.BatchMeshBuffer, {
				{DataType::UNSIGNED_BYTE, 3},
				{DataType::UNSIGNED_BYTE, 4},
			});

		s_Data.BatchDataBuffer = s_Device->CreateBuffer(sizeof(BatchData) * MAX_BATCH_MESHES, BufferType::ARRAY, BufferUsage::DYNAMIC_DRAW);
		s_Device->SetBufferLayout(s_Data.BatchDataBuffer, {
			{DataType::FLOAT, 4, 1},
			{DataType::FLOAT, 4, 1},
			{DataType::FLOAT, 4, 1},
			{DataType::FLOAT, 4, 1},
		}, s_Data.BatchMeshBuffer);
		
		s_Data.BatchCurrentOffset = 0;

		std::vector<std::string> meshShaderAttribs;
		meshShaderAttribs.push_back("aPos");
		meshShaderAttribs.push_back("aData");
		s_Data.MeshShader = s_Device->CreateShader(
			File::LoadTextFile("shader\\GL46\\main.vs"),
			File::LoadTextFile("shader\\GL46\\main.fs"),
			meshShaderAttribs
		);
		glm::mat4 identity(1.0f);
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)s_Data.RenderWidth / (float)s_Data.RenderHeight, 0.1f, 400.0f);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uModel", identity);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uView", identity);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uProj", proj);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uAtlas", 1);

		glActiveTexture(GL_TEXTURE1);
		Image atlas = File::LoadImage("image\\atlas.png");
		HTexture tex = s_Device->CreateTexture(atlas.GetWidth(), atlas.GetHeight(), atlas.GetFormat(), DataType::UNSIGNED_BYTE, ClampMode::CLAMP_TO_EDGE, FilterMode::NEAREST);
		s_Device->BindTexture(tex);
		s_Device->UploadTextureData(tex, atlas.GetData());
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
		 
		s_Device->Clear();
	}

	void Renderer::EndFrame() {
		//s_Device->BindShader(s_Data.PPShader);
		//s_Device->BindBuffer(s_Data.PPQuadBuffer);
		//s_Device->DrawArrays(DrawMode::TRIANGLE_STRIP, 0, 4);

		s_Device->BindShader(s_Data.MeshShader);
		s_Device->BindBuffer(s_Data.BatchMeshBuffer);
		s_Device->BindBuffer(s_Data.RenderCommandBuffer);
		s_Device->MultiDrawArraysIndirect(DrawMode::TRIANGLES, s_Data.RenderCommands.size());

		#ifdef ZC_PLATFORM_ZUNE
		ZDKGL_EndDraw();
		#endif
	}

	HMesh Renderer::BatchSubmitMesh(PolyMesh& mesh, const glm::vec3& translation) {
		RenderCommand command;
		command.Count = mesh.GetVertexCount();
		command.InstanceCount = 1;
		command.First = (s_Data.BatchCurrentOffset / sizeof(Vertex));
		command.BaseInstance = s_Data.BatchData.size();
		
		s_Data.RenderCommands.push_back(command);
		s_Device->BufferData(s_Data.RenderCommandBuffer, s_Data.RenderCommands.size() * sizeof(RenderCommand), 0, s_Data.RenderCommands.data());

		BatchData data;
		data.Translation = glm::mat4(1.0f);
		data.Translation = glm::translate(data.Translation, translation);
		s_Data.BatchData.push_back(data);
		s_Device->BufferData(s_Data.BatchDataBuffer, s_Data.BatchData.size() * sizeof(BatchData), 0, s_Data.BatchData.data());

		s_Device->BufferData(s_Data.BatchMeshBuffer, mesh.GetSizeInBytes(), s_Data.BatchCurrentOffset, mesh.GetVertexStream());
		s_Data.BatchCurrentOffset += mesh.GetSizeInBytes();

		return HMesh(0);
	}

	void Renderer::BatchDrawMesh(HMesh hMesh) {

	}

	void Renderer::BatchFreeMesh(HMesh hMesh) {

	}

	void Renderer::DrawDebugCube(const glm::vec3& translation, const glm::vec3& size, const glm::vec3& color) {

	}
}