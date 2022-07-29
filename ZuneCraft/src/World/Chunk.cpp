#include "Core/Base.h"
#include "World/Chunk.h"
#include "Graphics/GL.h"
#include "Geometry/Mesher.h"
#include "Graphics/Renderer.h"

#include <time.h>
#include <fastnoise/FastNoiseLite.h>

namespace ZuneCraft {
	Chunk::Chunk(const glm::ivec2& index) {
		SetIndex(index);
		GenTerrain();
	}

	void Chunk::SetIndex(const glm::ivec2& index) {
		m_Index = index;
		m_WorldPostion = glm::vec3(index.x * CHUNK_WIDTH, 0, index.y * CHUNK_WIDTH);
	}

	const glm::ivec2& Chunk::GetIndex() const{
		return m_Index;
	}

	void Chunk::GenTerrain() {
		memset(&m_Voxels.Data[0], 0, CHUNK_SIZE_QUBED);
		FastNoiseLite noise;
		noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		
		for (int x = 0; x < CHUNK_WIDTH; x++) {
			for (int z = 0; z < CHUNK_WIDTH; z++) {

				float w1 = noise.GetNoise((x + m_WorldPostion.x) / 16.0f, (z + m_WorldPostion.z) / 16.0f);
				float w2 = noise.GetNoise(x + m_WorldPostion.x, z + m_WorldPostion.z);
				float w3 = noise.GetNoise((x + m_WorldPostion.x) * 16.0f, (z + m_WorldPostion.z) * 16.0f);
				float w4 = noise.GetNoise((x + m_WorldPostion.x) / 64.0f, (z + m_WorldPostion.z) / 64.0f);

				int height = (w1 + w2 + w3 * w4) * CHUNK_HEIGHT;
				for (int y = 0; y < height && y < CHUNK_HEIGHT; y++) {
					if (y == height - 1) {
						m_Voxels.Data[x][y][z] = GRASS;
					}
					else {
						m_Voxels.Data[x][y][z] = DIRT;
					}
				}

				m_Voxels.Data[x][0][z] = GRASS;
			}
		}
	}

	Chunk::~Chunk() {

	}

	const glm::vec3& Chunk::GetWorldPosition() const {
		return m_WorldPostion;
	}

	const glm::vec3 Chunk::GetWorldPositionCentered() const {
		return m_WorldPostion + glm::vec3(CHUNK_WIDTH / 2, 0, CHUNK_WIDTH / 2);
	}

	uint8_t Chunk::TryGet(int x, int y, int z) {
		if (x >= CHUNK_WIDTH || x < 0) {
			return AIR;
		}

		if (y >= CHUNK_HEIGHT || y < 0) {
			return AIR;
		}

		if (z >= CHUNK_WIDTH || z < 0) {
			return AIR;
		}

		return m_Voxels.Data[x][y][z];

	}

	bool Chunk::IsVoidAt(int x, int y, int z) {
		return m_Voxels.Data[x][y][z] == AIR;
	}

	void Chunk::Update() {
		std::vector<Vertex> mesh;
		Mesher::VoxelToGreedy(m_Voxels, &mesh);
		Renderer::BatchSubmitMesh(mesh, GetWorldPosition());
	}
}