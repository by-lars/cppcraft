#include "World/World.h"
#include "Graphics/Renderer.h"

namespace ZuneCraft {
	World::World() {
		m_MapOffset = glm::ivec2(0);
		m_LastMapOffset = glm::ivec2(0);

		delta = glm::ivec2(0);
		rowToLoad = glm::ivec2(0);
		rowToUnload = glm::ivec2(0);
	}

	void World::Initialize() {
		/*for (int x = m_MapOffset.x - VIEW_DISTANCE_CHUNKS + 1; x < m_MapOffset.x + VIEW_DISTANCE_CHUNKS; x++) {
			for (int y = m_MapOffset.y - VIEW_DISTANCE_CHUNKS + 1; y < m_MapOffset.y + VIEW_DISTANCE_CHUNKS; y++) {
				glm::ivec2 index(x, y);
				Chunk* newChunk = new Chunk(index);
				newChunk->Update();
				m_Chunks.push_back(newChunk);
				m_ChunkMap[index] = newChunk;
			}
		}*/
	}

	void World::Render() {
		for (int x = 0; x < m_Chunks.size(); x++) {
				Chunk* chunk = m_Chunks[x];
				//Renderer::DrawMesh(chunk->GetBuffer(), chunk->GetVertCount(), chunk->GetWorldPosition());		
		}
	}

	void World::LoadPendingChunks() {
		//if (m_ChunksToLoad.empty() || m_FreeChunks.empty()) {
		//	return;
		//}

		//Chunk* chunk = m_FreeChunks.back();
		//m_FreeChunks.pop_back();

		//glm::ivec2 location = m_ChunksToLoad.back();
		//m_ChunksToLoad.pop_back();

		//chunk->SetIndex(location);
		//chunk->GenTerrain();
		//chunk->Update();

		//m_Chunks.push_back(chunk);
		//m_ChunkMap[location] = chunk;
	}

	void World::SetPlayerPos(glm::vec3 pos, const glm::vec3& lookDir) {
		//glm::ivec2 playerChunkPos = ToChunkCoords(pos);
		//Renderer::DrawDebugCube(ToWorldCoords(playerChunkPos) + glm::vec3(CHUNK_WIDTH/2, 0, CHUNK_WIDTH/2), glm::vec3(32.0, 1.0, 32.0), glm::vec3(0.0, 1.0, 1.0));

		//m_LastMapOffset = m_MapOffset;
		//m_MapOffset = playerChunkPos;

		//for (int x = m_MapOffset.x - VIEW_DISTANCE_CHUNKS + 1; x < m_MapOffset.x + VIEW_DISTANCE_CHUNKS; x++) {
		//	for (int y = m_MapOffset.y - VIEW_DISTANCE_CHUNKS + 1; y < m_MapOffset.y + VIEW_DISTANCE_CHUNKS; y++) {
		//		Renderer::DrawDebugCube(ToWorldCoords(glm::ivec2(x, y)) + glm::vec3(CHUNK_WIDTH / 2, 0, CHUNK_WIDTH / 2), glm::vec3(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		//	}
		//}

		//if (m_LastMapOffset != m_MapOffset) {
		//	delta = m_MapOffset - m_LastMapOffset;


		//	rowToLoad = m_MapOffset + delta * (VIEW_DISTANCE_CHUNKS - 1);
		//	rowToUnload = m_LastMapOffset - delta * (VIEW_DISTANCE_CHUNKS - 1);


		//	//Load in y direction
		//	if (rowToLoad.x != m_MapOffset.x) {
		//		for (int y = rowToLoad.y - VIEW_DISTANCE_CHUNKS + 1; y < rowToLoad.y + VIEW_DISTANCE_CHUNKS; y++) {
		//			glm::ivec2 index(rowToLoad.x, y);
		//			m_ChunksToLoad.push_back(index);
		//		}
		//	}

		//	//Load in x direction
		//	if (rowToLoad.y != m_MapOffset.y) {
		//		for (int x = rowToLoad.x - VIEW_DISTANCE_CHUNKS + 1; x < rowToLoad.x + VIEW_DISTANCE_CHUNKS; x++) {
		//			glm::ivec2 index(x, rowToLoad.y);
		//			m_ChunksToLoad.push_back(index);
		//		}
		//	}


		//	////UnLoad in x direction
		//	if (rowToUnload.y != m_MapOffset.y) {
		//		for (int x = rowToUnload.x - VIEW_DISTANCE_CHUNKS + 1; x < rowToUnload.x + VIEW_DISTANCE_CHUNKS; x++) {
		//			glm::ivec2 index(x, rowToUnload.y);
		//			Chunk* chunk = m_ChunkMap[index];

		//			if (chunk != NULL) {
		//				m_Chunks.erase(std::find(m_Chunks.begin(), m_Chunks.end(), chunk));
		//				m_ChunkMap.erase(index);
		//				m_FreeChunks.push_back(chunk);
		//			}
		//		}
		//	}

		//	//UnLoad in y direction
		//	if (rowToUnload.x != m_MapOffset.x) {
		//		for (int y = rowToUnload.y - VIEW_DISTANCE_CHUNKS + 1; y < rowToUnload.y + VIEW_DISTANCE_CHUNKS; y++) {
		//			glm::ivec2 index(rowToUnload.x, y);
		//			Chunk* chunk = m_ChunkMap[index];

		//			if (chunk != NULL) {
		//				m_Chunks.erase(std::find(m_Chunks.begin(), m_Chunks.end(), chunk));
		//				m_ChunkMap.erase(index);
		//				m_FreeChunks.push_back(chunk);
		//			}
		//		}
		//	}

		//}

		//Renderer::DrawDebugCube(ToWorldCoords(rowToLoad) + glm::vec3(CHUNK_WIDTH / 2, 0, CHUNK_WIDTH / 2), glm::vec3(2.0), glm::vec3(0.0, 1.0, 0.0));
		//Renderer::DrawDebugCube(ToWorldCoords(rowToUnload) + glm::vec3(CHUNK_WIDTH / 2, 0, CHUNK_WIDTH / 2), glm::vec3(2.0), glm::vec3(1.0, 0.0, 0.0));

	}
}