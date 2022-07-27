#pragma once

#include "World/Chunk.h"
#include "Utility/Hash.h"

#include <vector>
#include <deque>
#include <glm/glm.hpp>
#include <boost/unordered/unordered_map.hpp>

namespace ZuneCraft {
	class World {
	public:
		World();

		void SetPlayerPos(glm::vec3 pos, const glm::vec3& lookDir);

		void LoadPendingChunks();

		void Initialize();
		void Render();

		inline glm::ivec2 ToChunkCoords(const glm::vec3& pos) { return glm::ivec2(floor(pos.x / (float)CHUNK_WIDTH), floor(pos.z / (float)CHUNK_WIDTH)); };
		inline glm::vec3 ToWorldCoords(const glm::ivec2& pos) { return glm::vec3(pos.x * (float)CHUNK_WIDTH, 0, pos.y * (float)CHUNK_WIDTH); };

	private:
		static const int VIEW_DISTANCE_CHUNKS = 3;
		static const int VIEW_DISTANCE = VIEW_DISTANCE_CHUNKS * CHUNK_WIDTH;

		std::vector<Chunk*> m_Chunks;
		std::vector<glm::ivec2> m_ChunksToLoad;
		std::vector<Chunk*> m_FreeChunks;

		glm::ivec2 m_MapOffset;
		glm::ivec2 m_LastMapOffset;

		boost::unordered_map<glm::ivec2, Chunk*> m_ChunkMap;
	
		glm::ivec2 delta;
		glm::ivec2 rowToLoad;
		glm::ivec2 rowToUnload;
	};
}