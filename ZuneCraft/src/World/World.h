#pragma once

#include "World/Chunk.h"
#include <vector>
#include <deque>
#include <glm/glm.hpp>

namespace ZuneCraft {
	class World {
	public:
		World();
		~World();

		void SetPlayerPos(glm::vec3 pos, const glm::vec3& lookDir);

		void Render();

		static inline glm::ivec2 ToChunkCoords(const glm::vec3& pos) { return glm::ivec2(floor(pos.x / (float)CHUNK_WIDTH), floor(pos.z / (float)CHUNK_WIDTH)); };
		static inline glm::vec3 ToWorldCoords(const glm::ivec2& pos) { return glm::vec3(pos.x * (float)CHUNK_WIDTH, 0, pos.y * (float)CHUNK_WIDTH); };

	private:
		static const int VIEW_DISTANCE_CHUNKS = 6;
		static const int VIEW_DISTANCE_BLOCKS = VIEW_DISTANCE_CHUNKS * CHUNK_WIDTH;

		std::vector<Chunk*> m_Chunks;

		glm::ivec2 m_LastChunk;
		glm::ivec2 m_CurrentChunk;

	};
}