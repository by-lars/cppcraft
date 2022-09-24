#pragma once

#include "Core/Service.h"
#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexPool.h"
#include "Data/HandleStore.h"
#include "Graphics/RenderAPI.h"

#include <glm/glm.hpp>
#include <queue>
#include <vector>

namespace ZuneCraft {
	class Renderer : public IService {
	public:
		Renderer();
		~Renderer();

		void Init() override;
		void Shutdown();

		void BeginFrame();
		void EndFrame();

		void SetResolution(int width, int height);
		void SetView(const glm::mat4& viewMat);
		void SetFlip(bool flipped);

		Id BatchSubmitMesh(std::vector<Vertex>& mesh, const glm::vec3& translation);
		void BatchDrawMesh(Id hMesh);
		void BatchFreeMesh(Id hMesh);

	private:
		RenderAPI* m_Device;

		struct BatchData {
			glm::vec3 Translation;
		};

		struct MeshInfo {
			VPNode* MemoryLocation;
			size_t BatchId;
			Id RenderCommand;
		};

		GLuint m_RenderWidth;
		GLuint m_RenderHeight;

		//PostProcessing
		Id m_PPShader;
		Id m_PPQuadBuffer;
		Id m_PPRenderTarget;

		//Mesh Rendering
		Id m_BatchDataBuffer;
		Id m_MeshShader;
		size_t m_BatchCurrentOffset;

		std::vector<BatchData> m_BatchData;
		VertexPool<Vertex>* m_BatchMesh;
		std::queue<size_t> m_BatchDataFreeSlots;

		HandleStore<MeshInfo> m_Meshes;
	};
}