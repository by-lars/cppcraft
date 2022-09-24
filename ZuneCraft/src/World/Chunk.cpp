#include "Core/Base.h"
#include "Core/Service.h"
#include "Build/PlatformDefines.h"
#include "World/Chunk.h"
#include "Graphics/GL.h"
#include "Graphics/Renderer.h"
#include "Threading/ThreadPool.h"

#include <time.h>
#include <fastnoise/FastNoiseLite.h>

namespace ZuneCraft {
	Chunk::Chunk() {
		m_MeshHandle = Handle::INVALID;
		m_Renderer = Services::Get<Renderer>();
		m_ThreadPool = Services::Get<ThreadPool>();
		m_State = State::UNLOADED;
		ZC_DEBUG("m_Renderer=" << m_Renderer);
	}

	Chunk::~Chunk() {
		ZC_DEBUG_ALLOC("Deleting Chunk with MeshID=" << Handle::GetIndex(m_MeshHandle));
		if (Handle::IsValid(m_MeshHandle)) {
			m_Renderer->BatchFreeMesh(m_MeshHandle);
		}
	}

	void Chunk::OnMeshBuilt(void* context) {
		Chunk* chunk = (Chunk*)context;
		chunk->m_MeshHandle = chunk->m_Renderer->BatchSubmitMesh(chunk->m_Mesh, chunk->GetWorldPosition());
		chunk->m_State = State::LOADED;
		chunk->m_Mesh.clear();
		chunk->m_Mesh.resize(0);
	}

	void Chunk::SetIndex(const glm::ivec2& index) {
		m_Index = index;
		m_WorldPostion = glm::vec3(index.x * WIDTH, 0, index.y * WIDTH);
	}

	const glm::ivec2& Chunk::GetIndex() const{
		return m_Index;
	}

	void Chunk::Tick() {
		switch (m_State)
		{
		case Chunk::State::UNLOADED:

			break;

		case Chunk::State::BUILDING:

			break;

		case Chunk::State::BUILT:
			break;

		case Chunk::State::LOADED:

			break;
		}
	}

	void Chunk::Load(const glm::ivec2& index) {
		SetIndex(index);

		GenTerrain();

		m_State = State::BUILDING;

		ThreadPool::Job job;
		job.JobFunction = GenMesh;
		job.CallbackFunction = OnMeshBuilt;
		job.Context = this;
		m_ThreadPool->SubmitWork(job);
	}

	void Chunk::Unload() {
		if (Handle::IsValid(m_MeshHandle)) {
			m_Renderer->BatchFreeMesh(m_MeshHandle);
		}
	}

	void Chunk::GenMesh(void* context) {
		static int CHUNK_DIMENSIONS[3] = { Chunk::WIDTH , Chunk::HEIGHT, Chunk::WIDTH};
		
		Chunk* chunk = (Chunk*)context;
		
		//std::vector<Vertex> mesh;
		//mesh.reserve(Chunk::VOLUME_SIZE);

		chunk->m_Mesh.clear();
		chunk->m_Mesh.reserve(Chunk::VOLUME_SIZE);

		Face* mask = new Face[Chunk::WIDTH * Chunk::HEIGHT];

		for (int d = 0; d < 3; d++) {
			int w, h, k;

			int x[3] = { 0 };
			int q[3] = { 0 }; // (1, 0, 0); (0, 1, 0); (0, 0, 1);

			int u = (d + 1) % 3; // 1 2 0
			int v = (d + 2) % 3; // 2 0 1

			q[d] = 1;

			int normal = 2 * (d % 2) - 1;

			memset(mask, 0, Chunk::AREA_SIZE);

			//Go through each slice of the chunk
			// (di, u, v); (v, di, u); (u, v, di);
			for (x[d] = -1; x[d] < CHUNK_DIMENSIONS[d];) {

				//Create the mask
				int n = 0;
				for (x[v] = 0; x[v] < CHUNK_DIMENSIONS[v]; x[v]++) {
					for (x[u] = 0; x[u] < CHUNK_DIMENSIONS[u]; x[u]++) {
						Material voxel = (x[d] >= 0) ? (Material)chunk->m_Voxels[x[0]][x[1]][x[2]] : Material::AIR;
						Material nextVoxel = (x[d] < CHUNK_DIMENSIONS[d] - 1) ? (Material)chunk->m_Voxels[x[0] + q[0]][x[1] + q[1]][x[2] + q[2]] : Material::AIR;

						bool isVoxelSolid = (voxel == Material::AIR);
						bool isNextVoxelSolid = (nextVoxel == Material::AIR);
						bool isBackFace = (isNextVoxelSolid == false);
					//	bool isBackFace = x[v] % 2;
						Material type = isNextVoxelSolid ? voxel : nextVoxel;

						Face face(isVoxelSolid != isNextVoxelSolid, isBackFace, type);
						mask[n++] = face;
					}
				}

				x[d]++;
				n = 0;

				//Generate the mesh from the mask
				for (int j = 0; j < CHUNK_DIMENSIONS[v]; j++) {
					for (int i = 0; i < CHUNK_DIMENSIONS[u];) {

						if (mask[n].IsSolid) {

							//Compute the width of the quad/run
							for (w = 1; i + w < CHUNK_DIMENSIONS[u] && mask[n + w].IsSolid && mask[n] == mask[n + w]; w++) {}

							bool done = false;
							for (h = 1; j + h < CHUNK_DIMENSIONS[v]; h++) {
								for (k = 0; k < w; k++) {
									Face f = mask[n + k + h * CHUNK_DIMENSIONS[u]];
									if (f.IsSolid == false || f != mask[n]) {
										done = true;
										break;
									}
								}

								if (done)
									break;
							}

							x[u] = i;
							x[v] = j;

							int du[3] = { 0 }; du[u] = w;
							int dv[3] = { 0 }; dv[v] = h;
							normal = mask[n].IsBackFace;
							if (mask[n].IsBackFace) {
								chunk->m_Mesh.push_back(Vertex(x[0] + du[0],			x[1] + du[1],			x[2] + du[2],			w, h, (uint8_t)mask[n].Type,	normal)); //Top Right
								chunk->m_Mesh.push_back( Vertex(x[0],					x[1],					x[2],					0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
								chunk->m_Mesh.push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1],	x[2] + du[2] + dv[2],	w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								chunk->m_Mesh.push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1],	x[2] + du[2] + dv[2],	w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								chunk->m_Mesh.push_back( Vertex(x[0],					x[1],					x[2],					0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
								chunk->m_Mesh.push_back( Vertex(x[0] + dv[0],			x[1] + dv[1],			x[2] + dv[2],			0, 0, (uint8_t)mask[n].Type, normal)); //Lower left cornor
							}
							else {
								chunk->m_Mesh.push_back( Vertex(x[0] + du[0], x[1] + du[1], x[2] + du[2], w, h, (uint8_t)mask[n].Type, normal)); //Top Right
								chunk->m_Mesh.push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2], w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								chunk->m_Mesh.push_back( Vertex(x[0], x[1], x[2], 0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
								chunk->m_Mesh.push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2], w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								chunk->m_Mesh.push_back( Vertex(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2], 0, 0, (uint8_t)mask[n].Type, normal)); //Lower left cornor
								chunk->m_Mesh.push_back( Vertex(x[0], x[1], x[2], 0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
							}

							for (int l = 0; l < h; l++)
								for (int f = 0; f < w; f++)
									mask[n + f + l * CHUNK_DIMENSIONS[u]].IsSolid = false;

							i += w;
							n += w;

						}
						else {
							i++;
							n++;
						}
					}
				}
			}
		}

		delete[] mask;
		
	//	m_MeshHandle = m_Renderer->BatchSubmitMesh(mesh, GetWorldPosition());
	}

	void Chunk::GenTerrain() {
		memset(&m_Voxels[0], 0, VOLUME_SIZE);
		FastNoiseLite noise;

		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		noise.SetFractalOctaves(8);
		noise.SetFractalLacunarity(2.0f);
		noise.SetFractalGain(0.5f);
		noise.SetFrequency(0.005f);

		for (int x = 0; x < WIDTH; x++) {
			for (int z = 0; z < HEIGHT; z++) {

	
				float height = noise.GetNoise((x + m_WorldPostion.x), (z + m_WorldPostion.z));
				height = powf(height, 1.5f);


				int blockHeight = int(height * HEIGHT);

				//blockHeight = 5;

				for (int y = 0; y < blockHeight && y < HEIGHT; y++) {

					if (y == (int)blockHeight - 1) {
						m_Voxels[x][y][z] = (uint8_t)Material::GRASS;
					}
					else {
						m_Voxels[x][y][z] = (uint8_t)Material::DIRT;
					}
					
				}

				m_Voxels[x][0][z] = (uint8_t)Material::GRASS;
			}
		}
	}

	const glm::vec3& Chunk::GetWorldPosition() const {
		return m_WorldPostion;
	}

	const glm::vec3 Chunk::GetWorldPositionCentered() const {
		return m_WorldPostion + glm::vec3(WIDTH / 2, 0, WIDTH / 2);
	}

}