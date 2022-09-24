#pragma once

#include "World/Chunk.h"
#include "Data/Handle.h"
#include "World/Player.h"

#include <vector>
#include <deque>
#include <glm/glm.hpp>

namespace ZuneCraft {
	class Renderer;

	class World {
	public:
		World();
		~World();


		void Tick();
		void Render();

		static inline glm::ivec2 ToChunkCoords(const glm::vec3& pos) { return glm::ivec2(floor(pos.x / (float)Chunk::WIDTH), floor(pos.z / (float)Chunk::WIDTH)); };
		static inline glm::vec3 ToWorldCoords(const glm::ivec2& pos) { return glm::vec3(pos.x * (float)Chunk::WIDTH, 0, pos.y * (float)Chunk::WIDTH); };

	private:
		void LoadChunks(glm::vec3 pos, bool forceLoad = false);

		Renderer* m_Renderer;

		Player m_Player;

		std::vector<Chunk*> m_Chunks;
		std::vector<Chunk*> m_FreeChunks;

		glm::ivec2 m_LastChunk;
		glm::ivec2 m_CurrentChunk;

	};
}