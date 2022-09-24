#pragma once
#include "Core/Base.h"
#include "World/Material.h"
#include "Data/Handle.h"
#include "Graphics/Vertex.h"
#include <glm/glm.hpp>
#include <vector>

namespace ZuneCraft {
	class Renderer;
	class ThreadPool;

	class Chunk {
	public:
		const static int WIDTH = 32;
		const static int HEIGHT = 128;
		const static int VOLUME_SIZE = WIDTH * WIDTH * HEIGHT;
		const static int AREA_SIZE = WIDTH * WIDTH;

		ZC_ENUM State {
			UNLOADED = 0,
			BUILDING,
			BUILT,
			LOADED
		};

		Chunk();
		~Chunk();

		static void OnMeshBuilt(void* context);

		const glm::ivec2& GetIndex() const;
		void SetIndex(const glm::ivec2& index);

		const glm::vec3& GetWorldPosition() const;
		const glm::vec3 GetWorldPositionCentered() const;

		void Load(const glm::ivec2& index);
		void Unload();

		void Tick();

	private:
		void GenTerrain();
		static void GenMesh(void* context);

		State m_State;

		uint8_t m_Voxels[WIDTH][HEIGHT][WIDTH];
		//Face m_Mask[Chunk::WIDTH * Chunk::HEIGHT];
		std::vector<Vertex> m_Mesh;

		Id m_MeshHandle;

		Renderer* m_Renderer;
		ThreadPool* m_ThreadPool;

		glm::ivec2 m_Index;
		glm::vec3 m_WorldPostion;
	};
}
