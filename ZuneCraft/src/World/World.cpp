#include "World/World.h"
#include "Graphics/Renderer.h"
#include "Core/Game.h"

#include <fastnoise/FastNoiseLite.h>

namespace ZuneCraft {
	World::World() {
		m_LastChunk = glm::ivec2(0, 0);
		m_CurrentChunk = glm::ivec2(0, 0);


		for (int x = 0; x < Game::Get().GetConfig().RenderDistance; x++) {
			for (int y = 0; y < Game::Get().GetConfig().RenderDistance; y++) {
				m_Chunks.push_back( new Chunk() );
			}
		}

		SetPlayerPos(glm::vec3(123,123,123), glm::vec3(0));
	}

	World::~World() {

	}

	void World::Render() {

	}


	void World::SetPlayerPos(glm::vec3 pos, const glm::vec3& lookDir) {
		pos.y = 0;
		m_CurrentChunk = ToChunkCoords(pos);

		if (m_CurrentChunk - m_LastChunk == glm::ivec2(0, 0)) {
			return; //same chunk, don't need to load new chunks
		}

		ZC_DEBUG("Load Chunks");

		std::vector<Chunk*>::iterator it = m_Chunks.begin();
		while (it != m_Chunks.end()) {
			float distance = glm::distance((*it)->GetWorldPositionCentered(), pos);

			if (distance > Game::Get().GetConfig().RenderDistance * Chunk::WIDTH) {
				(*it)->Unload();
				m_FreeChunks.push_back(*it);
				it = m_Chunks.erase(it);
			}
			else {
				it++;
			}
		}

		glm::ivec2 chLoadStart = m_CurrentChunk - glm::ivec2(Game::Get().GetConfig().RenderDistance / 2);
		glm::ivec2 chLoadEnd = m_CurrentChunk + glm::ivec2(Game::Get().GetConfig().RenderDistance / 2);

		std::vector<glm::ivec2> chunksToLoad;

		for (int x = chLoadStart.x; x < chLoadEnd.x; x++) {
			for (int y = chLoadStart.y; y < chLoadEnd.y; y++) {
				chunksToLoad.push_back(glm::ivec2(x, y));
				//ZC_DEBUG("Consider chunk at x" << x << " y" << y);
			}
		}

		for (int i = 0; i < chunksToLoad.size(); i++) {
			bool chunkAlreadyLoaded = false;

			for (int j = 0; j < m_Chunks.size(); j++) {
				if (m_Chunks[j]->GetIndex() == chunksToLoad[i]) {
					chunkAlreadyLoaded = true;
					break;
				}
			}

			if (chunkAlreadyLoaded) {
				continue;
			}

			Chunk* chunk = nullptr;

			if (m_FreeChunks.empty()) {
				chunk = new Chunk();
			}
			else {
				chunk = m_FreeChunks.back();
				m_FreeChunks.pop_back();
			}


			chunk->Load(chunksToLoad[i]);
			m_Chunks.push_back(chunk);
			
		}


		m_LastChunk = m_CurrentChunk;
	}
}