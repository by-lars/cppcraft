#pragma once

#include "World/Chunk.h"
#include "Data/Handle.h"
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

		static inline glm::ivec2 ToChunkCoords(const glm::vec3& pos) { return glm::ivec2(floor(pos.x / (float)Chunk::WIDTH), floor(pos.z / (float)Chunk::WIDTH)); };
		static inline glm::vec3 ToWorldCoords(const glm::ivec2& pos) { return glm::vec3(pos.x * (float)Chunk::WIDTH, 0, pos.y * (float)Chunk::WIDTH); };

	private:
		std::vector<Chunk*> m_Chunks;
		std::vector<Chunk*> m_FreeChunks;

		glm::ivec2 m_LastChunk;
		glm::ivec2 m_CurrentChunk;

	};
}