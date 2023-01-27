#pragma once
#include "Core/Base.h"
#include "World/Material.h"
#include "Data/Handle.h"
#include "Data/Vertex.h"
#include <glm/glm.hpp>
#include <vector>

namespace ZuneCraft {
	class Renderer;
	class ThreadPool;

	typedef glm::ivec3 ChunkCoords;
	typedef glm::vec3 WorldCoords;
	typedef glm::ivec2 ChunkIndex;

	class Chunk {
	public:
		const static int WIDTH = 32;
		const static int HEIGHT = 255;
		const static int VOLUME_SIZE = WIDTH * WIDTH * HEIGHT;
		const static int AREA_SIZE = WIDTH * WIDTH;

		enum class State {
			UNLOADED = 0,
			BUILDING,
			BUILT,
			LOADED
		};

		Chunk();
		~Chunk();

		const ChunkIndex& GetIndex() const;
		const WorldCoords& GetWorldPosition() const;
		WorldCoords GetWorldPositionCentered() const;

		Material GetVoxel(const ChunkCoords& pos);

		void Load(const glm::ivec2& index);
		void Unload();

		void Tick();

	private:
		void GenTerrain();
		void SetIndex(const glm::ivec2& index);
		static void OnMeshBuilt(void* context);
		static void GenMesh(void* context);

		State m_State;

		uint8_t m_Voxels[WIDTH][HEIGHT][WIDTH];

		std::vector<Vertex> m_Mesh;

		Id m_MeshHandle;

		Renderer* m_Renderer;
		ThreadPool* m_ThreadPool;

		ChunkIndex m_Index;
		WorldCoords m_WorldPostion;
	};
}
