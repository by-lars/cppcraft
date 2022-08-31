#include "World/World.h"
#include "Graphics/Renderer.h"

namespace ZuneCraft {
	World::World() {
		m_LastChunk = glm::ivec2(0, 0);
		m_CurrentChunk = glm::ivec2(0, 0);



		glm::ivec2 chLoadEnd = glm::ivec2((VIEW_DISTANCE_CHUNKS / 2));
		glm::ivec2 chLoadStart = chLoadEnd * -1;

		for (int x = chLoadStart.x; x < chLoadEnd.x; x++) {
			for (int y = chLoadStart.y; y < chLoadEnd.y; y++) {
				Chunk* chunk = new Chunk(glm::ivec2(x,y));
				chunk->Update();
				m_Chunks.push_back(chunk);

			}
		}

	}

	World::~World() {
		for (int i = 0; i < m_Chunks.size(); i++) {
			delete m_Chunks[i];
		}
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

			if (distance > VIEW_DISTANCE_BLOCKS) {
				delete (*it);
				it = m_Chunks.erase(it);
			}
			else {
				it++;
			}
		}

		glm::ivec2 chLoadStart = m_CurrentChunk - (VIEW_DISTANCE_CHUNKS / 2);
		glm::ivec2 chLoadEnd = m_CurrentChunk + (VIEW_DISTANCE_CHUNKS / 2);

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

			Chunk* chunk = new Chunk(chunksToLoad[i]);
			chunk->Update();
			m_Chunks.push_back(chunk);
		}


		m_LastChunk = m_CurrentChunk;
	}
}