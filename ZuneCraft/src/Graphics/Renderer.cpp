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
#include "Utility/Asset.h"

#ifdef ZC_PLATFORM_ZUNE
#include <zdk.h> 
#endif

namespace ZuneCraft {
	static RenderAPI* s_Device = nullptr;

	static const size_t MAX_BATCH_MESHES = 500;

	struct BatchData {
		glm::vec3 Translation;
	};

	struct RenderData {
		//General
		GLuint RenderWidth;
		GLuint RenderHeight;

		//PostProcessing
		HShader PPShader;
		HBuffer PPQuadBuffer;

		//Mesh Rendering
		HBuffer BatchMeshBuffer;
		HBuffer BatchDataBuffer;
		HShader MeshShader;
		size_t BatchCurrentOffset;
		std::vector<BatchData> BatchData;
	};

	static RenderData s_Data;

	void Renderer::Init() {
		ZC_DEBUG("Initializing Renderer");
		s_Data.RenderWidth = Game::Get().GetWindow().GetWidth();
		s_Data.RenderHeight = Game::Get().GetWindow().GetHeight();

		s_Device = RenderAPI::Create();
		

		//Mesh Buffer
		std::vector<BufferElement> bufferElements;
		s_Data.BatchMeshBuffer = s_Device->CreateBuffer(CHUNK_SIZE_QUBED * MAX_BATCH_MESHES, BufferType::ARRAY, BufferUsage::DYNAMIC_DRAW);
		bufferElements.push_back(BufferElement(DataType::UNSIGNED_BYTE, 4, 0));
		bufferElements.push_back(BufferElement(DataType::UNSIGNED_BYTE, 4, 0));
		s_Device->SetBufferLayout(s_Data.BatchMeshBuffer, HBuffer::Invalid(), bufferElements);

		//Setup Renderer paths based on api capabilities
		RenderAPI::Capabilities apiCapabilities = s_Device->GetCapabilities();
		if (apiCapabilities.IndirectDrawing) {
			SetupIndirectRenderPath();
		} else {
			SetupDirectRenderPath();
		}

		//Setup default state
		s_Device->SetClearColor(0.51f, 0.64f, 1.0f, 1.0f);
		s_Device->SetViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);

		//Setup Post Processing Shader
		std::vector<std::string> shaderAttribs; shaderAttribs.push_back("aPos");
		s_Data.PPShader = LoadShader("PostProcess", shaderAttribs);
		s_Device->SetShaderUniform(s_Data.PPShader, "uTexture", 0);
		SetFlip(false);

		//Setup Post Processing Fullscreen Quad
		s_Data.PPQuadBuffer = s_Device->CreateBuffer(sizeof(Mesher::FullscreenQuad), BufferType::ARRAY, BufferUsage::STATIC_DRAW);
		s_Device->BufferData(s_Data.PPQuadBuffer, sizeof(Mesher::FullscreenQuad), 0, (void*)&Mesher::FullscreenQuad[0]);
		bufferElements.clear();
		bufferElements.push_back(BufferElement(DataType::UNSIGNED_BYTE, 4, 0));
		s_Device->SetBufferLayout(s_Data.PPQuadBuffer, HBuffer::Invalid(), bufferElements);

		//Setup Default Mesh Shader Uniforms
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)s_Data.RenderWidth / (float)s_Data.RenderHeight, 0.1f, 400.0f);
		shaderAttribs.clear();
		shaderAttribs.push_back("aPos");
		shaderAttribs.push_back("aData");
		s_Data.MeshShader = LoadShader("main", shaderAttribs);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uProj", proj);
		s_Device->SetShaderUniform(s_Data.MeshShader, "uAtlas", 1);

		//Load Texture Atlas
		glActiveTexture(GL_TEXTURE1);
		Image atlas; Asset::GetImage("atlas.png", &atlas);
		HTexture tex = s_Device->CreateTexture(atlas.Width, atlas.Height, atlas.GetFormat(), DataType::UNSIGNED_BYTE, ClampMode::CLAMP_TO_EDGE, FilterMode::NEAREST);
		s_Device->UploadTextureData(tex, atlas.Data);
	}	

	void Renderer::SetupIndirectRenderPath() {
		//Setup Indirect Rendering
		s_Data.BatchDataBuffer = s_Device->CreateBuffer(sizeof(BatchData) * MAX_BATCH_MESHES, BufferType::ARRAY, BufferUsage::DYNAMIC_DRAW);
		std::vector<BufferElement> bufferElements;
		bufferElements.push_back(BufferElement(DataType::FLOAT, 3, 1));
		s_Device->SetBufferLayout(s_Data.BatchDataBuffer, s_Data.BatchMeshBuffer, bufferElements);
		s_Data.BatchCurrentOffset = 0;
	}

	void Renderer::SetupDirectRenderPath() {

	}

	HShader Renderer::LoadShader(const std::string& name, const std::vector<std::string>& attributes) {
		HShader shader;
		
		if (s_Device->GetCapabilities().ShaderCompiler) {
			shader = s_Device->CreateShader(Asset::GetShaderSource(name + ".vs"), Asset::GetShaderSource(name + ".fs"), attributes);
		}
		else {
			Binary vertex; Asset::GetShaderBinary(name + ".nvbv", &vertex);
			Binary fragment; Asset::GetShaderBinary(name + ".nvbf", &fragment);
			shader = s_Device->CreateShaderFromBinary(vertex, fragment, attributes);
		}

		return shader;
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
		s_Device->Flush();
	/*	s_Device->BindShader(s_Data.PPShader);
		s_Device->BindBuffer(s_Data.PPQuadBuffer);
		s_Device->DrawArrays(DrawMode::TRIANGLE_STRIP, 0, 4);*/

		s_Device->BindShader(s_Data.MeshShader);
		s_Device->BindBuffer(s_Data.BatchMeshBuffer);
		s_Device->MultiDrawArrays(DrawMode::TRIANGLES);

		#ifdef ZC_PLATFORM_ZUNE
		ZDKGL_EndDraw();
		#endif
	}

	HMesh Renderer::BatchSubmitMesh(std::vector<Vertex>& mesh, const glm::vec3& translation) {
		BatchData data;
		data.Translation = translation;
		s_Data.BatchData.push_back(data);

		RenderCommand command;
		command.Count = mesh.size();
		command.InstanceCount = 1;
		command.First = (s_Data.BatchCurrentOffset / sizeof(Vertex));
		command.BaseInstance = s_Data.BatchData.size() - 1;

		s_Device->PushRenderCommand(command);

		if (s_Device->GetCapabilities().IndirectDrawing) {
			s_Device->BufferData(s_Data.BatchDataBuffer, s_Data.BatchData.size() * sizeof(BatchData), 0, &s_Data.BatchData[0]);
		}
		else {
			for (int i = 0; i < mesh.size(); i++) {
				mesh[i].BatchIndex = s_Data.BatchData.size() - 1;
			}
			std::string name;
			name += "uTranslation[";
			name += std::to_string(s_Data.BatchData.size() - 1);
			name += "]";

			ZC_DEBUG(name);
			s_Device->SetShaderUniform(s_Data.MeshShader, name, translation);
		}

		s_Device->BufferData(s_Data.BatchMeshBuffer, mesh.size() * sizeof(Vertex), s_Data.BatchCurrentOffset, (void*)&mesh[0]);
		s_Data.BatchCurrentOffset += mesh.size() * sizeof(Vertex);

		return HMesh(0);
	}

	void Renderer::BatchDrawMesh(HMesh hMesh) {

	}

	void Renderer::BatchFreeMesh(HMesh hMesh) {

	}

	void Renderer::DrawDebugCube(const glm::vec3& translation, const glm::vec3& size, const glm::vec3& color) {

	}
}