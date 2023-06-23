#pragma once

#include "World/Chunk.h"
#include "Data/Handle.h"
#include "World/Player.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vector>
#include <deque>
#include <glm/glm.hpp>
#include <unordered_map>

namespace ZuneCraft {
	class Renderer;
	class ThreadPool;

	class World {
	public:
		World();
		~World();


		void Tick();
		void Render();

		Material GetVoxel(const WorldCoords& pos);
		static ChunkIndex ToChunkIndex(const WorldCoords& pos);
		static ChunkCoords ToChunkCoords(const WorldCoords& pos);
		static WorldCoords ToGrid(const WorldCoords& pos);

		//static inline glm::vec3 ToWorldCoords(const glm::ivec2& pos) { return glm::vec3(pos.x * (float)Chunk::WIDTH, 0, pos.y * (float)Chunk::WIDTH); };

	private:
		void LoadChunks(glm::vec3 pos, bool forceLoad);

		Renderer* m_Renderer;
		ThreadPool* m_ThreadPool;

		Player m_Player;

		std::vector<Chunk*> m_Chunks;
		std::vector<Chunk*> m_FreeChunks;
		std::unordered_map<ChunkIndex, Chunk*> m_ChunkMap;
	};
}