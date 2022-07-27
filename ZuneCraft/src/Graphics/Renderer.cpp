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

#ifdef ZC_PLATFORM_ZUNE
#include <zdk.h> 
#endif

namespace ZuneCraft {
	static RenderAPI* s_Device = nullptr;

	struct DebugCubeData {
		glm::mat4 ModelMatrix;
		glm::vec3 Color;
	};

	struct RenderData {
		//General
		GLuint RenderWidth;
		GLuint RenderHeight;

		//Mesh Rendering
		//HShader MeshShader;
		//HTexture BlockAtlas;

		//PostProcessing
		HShader PPShader;
		//GLuint PPFramebuffer;
		//GLuint PPRenderTexture;
		//GLuint PPDepthBuffer;
		HBuffer PPQuadBuffer;

		//Debug
		//HBuffer DebugCubeBuffer;
		//HShader DebugCubeShader;
		//std::vector<DebugCubeData> DebugCubeInstances;

		HBuffer TRIANGLES;
	};

	static RenderData s_Data;

	void Renderer::Init() {
		ZC_DEBUG("Initializing Renderer");
		s_Device = RenderAPI::Create();

		s_Device->SetClearColor(0.51f, 0.64f, 1.0f, 1.0f);
		glViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);

		s_Data.RenderWidth = Game::Get().GetWindow().GetWidth();
		s_Data.RenderHeight = Game::Get().GetWindow().GetHeight();

		//glActiveTexture(GL_TEXTURE1);
		//Image blockAtlasImage = File::LoadImage("image\\atlas.png");
		//s_Data.BlockAtlas = s_Device->CreateTexture(blockAtlasImage.GetWidth(), blockAtlasImage.GetHeight(), TextureFormat::RGBA, DataType::UNSIGNED_BYTE, ClampMode::CLAMP_TO_EDGE, FilterMode::NEAREST);
		//s_Device->UploadTextureData(s_Data.BlockAtlas, blockAtlasImage.GetData());

		//Setup main Shader
		//glm::mat4 identity(1.0f);
		//glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)s_Data.RenderWidth / (float)s_Data.RenderHeight, 0.1f, 400.0f);

		//std::vector<std::string> meshShaderAttribs;
		//meshShaderAttribs.push_back("aPos");
		//meshShaderAttribs.push_back("aData");
		//s_Data.MeshShader = s_Device->CreateShader(File::LoadTextFile("shader\\GL46\\main.vs"), File::LoadTextFile("shader\\GL46\\main.fs"), meshShaderAttribs);
		//s_Device->SetShaderUniform(s_Data.MeshShader, "uModel", identity);
		//s_Device->SetShaderUniform(s_Data.MeshShader, "uView", identity);
		//s_Device->SetShaderUniform(s_Data.MeshShader, "uProj", proj);
		//s_Device->SetShaderUniform(s_Data.MeshShader, "uAtlas", 1);

		//Setup PostProcessing
		//glGenFramebuffers(1, &s_Data.PPFramebuffer);
		//glBindFramebuffer(GL_FRAMEBUFFER, s_Data.PPFramebuffer);

		//glActiveTexture(GL_TEXTURE0);
		//HTexture renderTexture = s_Device->CreateTexture(s_Data.RenderWidth, s_Data.RenderHeight, TextureFormat::RGB, DataType::UNSIGNED_BYTE, ClampMode::CLAMP_TO_EDGE, FilterMode::NEAREST);

		//glGenRenderbuffers(1, &s_Data.PPDepthBuffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, s_Data.PPDepthBuffer);

#ifdef ZC_PLATFORM_ZUNE
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, s_Data.RenderWidth, s_Data.RenderHeight);
#else 
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, s_Data.RenderWidth, s_Data.RenderHeight);
#endif

		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_Data.PPDepthBuffer);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_Data.PPRenderTexture, 0);

		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		//	ZC_FATAL_ERROR("Could not create framebuffer!");
		//}

		//Setup Post Processing Shader
		std::vector<std::string> ppShaderAttribs;
		ppShaderAttribs.push_back("aPos");
		s_Data.PPShader = s_Device->CreateShader(File::LoadTextFile("shader\\GL46\\PostProcess.vs"), File::LoadTextFile("shader\\GL46\\PostProcess.fs"), ppShaderAttribs);
		s_Device->BindShader(s_Data.PPShader);
		s_Device->SetShaderUniform(s_Data.PPShader, "uTexture", 0);
		SetFlip(false);
		
		//Setup Fullscreen Quad
		//Texture coords are rotated, to render in landscape on a portrait buffer
#ifdef ZC_PLATFORM_ZUNE
		unsigned char quad[] = {
			0, 1, 0, 0, //top left
			0, 0, 1, 0, //bottom left
			1, 1, 0, 1, //top right
			1, 0, 1, 1 //bottom right
		};
#elif ZC_PLATFORM_WIN32
		unsigned char quad[] = {
			0, 1, 0, 1, //top left
			0, 0, 0, 0, //bottom left
			1, 1, 1, 1, //top right
			1, 0, 1, 0 //bottom right
		};
#endif

		//Setup Quad Buffer
		s_Data.PPQuadBuffer = s_Device->CreateBuffer(sizeof(quad), BufferType::ARRAY, BufferUsage::STATIC_DRAW);
		s_Device->BindBuffer(s_Data.PPQuadBuffer);
		s_Device->BufferData(s_Data.PPQuadBuffer, sizeof(quad), 0, &quad[0]);
		s_Device->SetBufferLayout(s_Data.PPQuadBuffer, {
			{DataType::UNSIGNED_BYTE, 4, 0}
		});

		//Debug Rendering
		//s_Data.DebugCubeBuffer = s_Device->CreateBuffer(sizeof(Mesher::LineCubeVertices), BufferType::ARRAY, BufferUsage::STATIC_DRAW);
		//s_Device->BufferData(s_Data.DebugCubeBuffer, sizeof(Mesher::LineCubeVertices), 0, (void*)&Mesher::LineCubeVertices[0]);
		//s_Device->SetBufferLayout(s_Data.DebugCubeBuffer, {
		//	{DataType::FLOAT, 3, 0}
		//});

		//std::vector<std::string> debugShaderAttribs;
		//debugShaderAttribs.push_back("aPos");
		//s_Data.DebugCubeShader = s_Device->CreateShader(File::LoadTextFile("shader\\GL46\\DebugShader.vs"), File::LoadTextFile("shader\\GL46\\DebugShader.fs"), debugShaderAttribs);
		//s_Device->SetShaderUniform(s_Data.DebugCubeShader, "uModel", proj);
		//s_Device->SetShaderUniform(s_Data.DebugCubeShader, "uView", proj);
		//s_Device->SetShaderUniform(s_Data.DebugCubeShader, "uProj", proj);

		float vertices[] = {
		   -0.5f, -0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 0.0f,
			0.0f,  0.5f, 0.0f, 0.0f
		};

		s_Data.TRIANGLES = s_Device->CreateBuffer(sizeof(vertices), BufferType::ARRAY, BufferUsage::STATIC_DRAW);
		s_Device->BindBuffer(s_Data.TRIANGLES);
		s_Device->BufferData(s_Data.TRIANGLES, sizeof(vertices), 0, &vertices[0]);
		s_Device->SetBufferLayout(s_Data.TRIANGLES,
			{ {DataType::FLOAT, 4, 0} }
		);

	}	

	void Renderer::Shutdown() {
		delete s_Device;
		//glDeleteFramebuffers(1, &s_Data.PPFramebuffer);
		//glDeleteTextures(1, &s_Data.PPRenderTexture);
		//glDeleteRenderbuffers(1, &s_Data.PPDepthBuffer);
	}

	void Renderer::SetView(const glm::mat4& viewMat) {
		//s_Device->SetShaderUniform(s_Data.DebugCubeShader, "uView", viewMat);
		//s_Device->SetShaderUniform(s_Data.MeshShader, "uView", viewMat);
	}

	void Renderer::SetResolution(int width, int height) {
#ifdef ZC_PLATFORM_WIN32
		//s_Data.RenderWidth = width;
		//s_Data.RenderHeight = height;

		//glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)s_Data.RenderWidth / (float)s_Data.RenderHeight, 0.1f, 400.0f);
		//s_Device->SetShaderUniform(s_Data.DebugCubeShader, "uView", proj);
		//s_Device->SetShaderUniform(s_Data.MeshShader, "uView", proj);

		//glBindTexture(GL_TEXTURE_2D, s_Data.PPRenderTexture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s_Data.RenderWidth, s_Data.RenderHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		//glViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);

		//glBindRenderbuffer(GL_RENDERBUFFER, s_Data.PPDepthBuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, s_Data.RenderWidth, s_Data.RenderHeight);
#endif
	}

	void Renderer::SetFlip(bool flipped) {
		s_Device->SetShaderUniform(s_Data.PPShader, "uFlip", flipped == true ? -1 : 1);
	}

	void Renderer::BeginFrame() {
		#ifdef ZC_PLATFORM_ZUNE
			ZDKGL_BeginDraw();
		#endif                  
		 
	//	#ifdef ZC_PLATFORM_WIN32
	//		if(Input::GetActionDebugWireframe())
	//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	#endif

		//Setup framebuffer for drawing to texture
		//glBindFramebuffer(GL_FRAMEBUFFER, s_Data.PPFramebuffer);
		//glViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);
		s_Device->Clear();
	}

	void Renderer::EndFrame() {
		//Render Debug Elements
		//if (Input::GetActionDebugGizmos()) {
		//	//glClear(GL_DEPTH_BUFFER_BIT);
		//	s_Device->BindBuffer(s_Data.DebugCubeBuffer);
		//	for (int i = 0; i < s_Data.DebugCubeInstances.size(); i++) {
		//		s_Device->SetShaderUniform(s_Data.DebugCubeShader, "uModel", s_Data.DebugCubeInstances[i].ModelMatrix);
		//		s_Device->SetShaderUniform(s_Data.DebugCubeShader, "uColor", s_Data.DebugCubeInstances[i].Color);
		//		s_Device->DrawArrays(DrawMode::LINES, 24, 0);
		//	}
		//}

		//#ifdef ZC_PLATFORM_WIN32
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//#endif
	
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//#ifdef ZC_PLATFORM_WIN32
			//glViewport(0, 0, s_Data.RenderWidth, s_Data.RenderHeight);
		//#elif ZC_PLATFORM_ZUNE
			//Swap Height and Width as the acctual screen buffer is in portrait mode
		//	glViewport(0, 0, s_Data.RenderHeight, s_Data.RenderWidth);
		//#endif

		//Draw buffer to fullscreen quad
		//s_Device->Clear();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*	s_Data.PPShader->Bind();
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.PPQuadBuffer);
		glVertexAttribPointer(0, 4, GL_UNSIGNED_BYTE, GL_FALSE, 4 * sizeof(unsigned char), (const void*)0);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/
		s_Device->BindShader(s_Data.PPShader);
		s_Device->BindBuffer(s_Data.PPQuadBuffer);
		s_Device->DrawArrays(DrawMode::TRIANGLE_STRIP, 0, 4);

		s_Device->BindBuffer(s_Data.TRIANGLES);
		s_Device->DrawArrays(DrawMode::TRIANGLES, 0, 3);

		#ifdef ZC_PLATFORM_ZUNE
		ZDKGL_EndDraw();
		#endif

		//s_Data.DebugCubeInstances.clear();
	}

	void Renderer::DrawMesh(GLuint vbo, GLuint count, const glm::vec3& translation) {
		//glm::mat4 model(1.0f);
		//model = glm::translate(model, translation);
		//s_Device->BindShader(s_Data.MeshShader);
		//s_Device->SetShaderUniform(s_Data.MeshShader, "uModel", model);

		//glBindBuffer(GL_ARRAY_BUFFER, vbo); //TODO: Check before binding buffer
		//Vertex::SetVertexFormat();
		//s_Device->DrawArrays(DrawMode::TRIANGLES, 0, count);

	}

	void Renderer::DrawDebugCube(const glm::vec3& translation, const glm::vec3& size, const glm::vec3& color) {
		//DebugCubeData data;
		//glm::mat4 model(1.0);
		//model = glm::translate(model, translation);
		//model = glm::scale(model, size);

		//data.ModelMatrix = model;
		//data.Color = color;

		//s_Data.DebugCubeInstances.push_back(data);
	}
}