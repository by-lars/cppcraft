#include "Geometry/Mesher.h"
#include "Geometry/Material.h"
#include "Geometry/Vertex.h"

namespace ZuneCraft {
	PolyMesh Mesher::VoxelToGreedy(const VoxelStorage& voxels) {
		int index = 0;
		PolyMesh mesh(CHUNK_SIZE_QUBED);

		for (int d = 0; d < 3; d++) {
			int w, h, k;
			int x[3] = { 0 };
			int q[3] = { 0 };
			int u = (d + 1) % 3;
			int v = (d + 2) % 3;
			q[d] = 1;
			Face* mask = new Face[CHUNK_WIDTH * CHUNK_HEIGHT];

			//Go through each slice of the chunk
			for (x[d] = -1; x[d] < CHUNK_DIMENSIONS[d];) {

				//Create the mask
				int n = 0;
				for (x[v] = 0; x[v] < CHUNK_DIMENSIONS[v]; x[v]++) {
					for (x[u] = 0; x[u] < CHUNK_DIMENSIONS[u]; x[u]++) {
						BlockType voxel = (x[d] >= 0) ? (BlockType)voxels.Data[x[0]][x[1]][x[2]] : AIR;
						BlockType nextVoxel = (x[d] < CHUNK_DIMENSIONS[d] - 1) ? (BlockType)voxels.Data[x[0] + q[0]][x[1] + q[1]][x[2] + q[2]] : AIR;

						bool isVoxelSolid = (voxel == AIR);
						bool isNextVoxelSolid = (nextVoxel == AIR);
						bool isBackFace = (isNextVoxelSolid == false);
						BlockType type = isNextVoxelSolid ? voxel : nextVoxel;
						uint8_t direction = d + 3 * isBackFace;

						Face face(isVoxelSolid != isNextVoxelSolid, isBackFace, type, direction);
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
							for (w = 1; i + w < CHUNK_DIMENSIONS[u] && mask[n + w].IsSolid && mask[n].Type == mask[n + w].Type; w++) {}

							bool done = false;
							for (h = 1; j + h < CHUNK_DIMENSIONS[v]; h++) {
								for (k = 0; k < w; k++) {
									Face f = mask[n + k + h * CHUNK_DIMENSIONS[u]];
									if (f.IsSolid == false || f.Type != mask[n].Type) {
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


							if (mask[n].IsBackFace) {
								mesh[index++] = Vertex(x[0] + du[0],			x[1] + du[1],			x[2] + du[2],			w, h, mask[n].Type,	mask[n].Direction); //Top Right
								mesh[index++] = Vertex(x[0],					x[1],					x[2],					0, h, mask[n].Type,	mask[n].Direction); //Top Left Cornor
								mesh[index++] = Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1],	x[2] + du[2] + dv[2],	w, 0, mask[n].Type,	mask[n].Direction); //Lower right cornor
								mesh[index++] = Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1],	x[2] + du[2] + dv[2],	w, 0, mask[n].Type,	mask[n].Direction); //Lower right cornor
								mesh[index++] = Vertex(x[0],					x[1],					x[2],					0, h, mask[n].Type,	mask[n].Direction); //Top Left Cornor
								mesh[index++] = Vertex(x[0] + dv[0],			x[1] + dv[1],			x[2] + dv[2],			0, 0, mask[n].Type,	mask[n].Direction); //Lower left cornor
							}
							else {
								mesh[index++] = Vertex(x[0] + du[0], x[1] + du[1], x[2] + du[2], w, h, mask[n].Type, mask[n].Direction); //Top Right
								mesh[index++] = Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2], w, 0, mask[n].Type, mask[n].Direction); //Lower right cornor
								mesh[index++] = Vertex(x[0], x[1], x[2], 0, h, mask[n].Type, mask[n].Direction); //Top Left Cornor
								mesh[index++] = Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2], w, 0, mask[n].Type, mask[n].Direction); //Lower right cornor
								mesh[index++] = Vertex(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2], 0, 0, mask[n].Type, mask[n].Direction); //Lower left cornor
								mesh[index++] = Vertex(x[0], x[1], x[2], 0, h, mask[n].Type, mask[n].Direction); //Top Left Cornor
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

		mesh.SetVertexCount(index);

		return mesh;
	}

	PolyMesh::PolyMesh(size_t vertexCount) {
		ZC_DEBUG("PolyMesh(size_t vertexCount)");
		m_VertexCount = vertexCount;
		m_VertexStream = new Vertex[vertexCount];
	}

	PolyMesh::PolyMesh(PolyMesh&& other) {
		m_VertexCount = other.m_VertexCount;
		m_VertexStream = other.m_VertexStream;

		other.m_VertexCount = 0;
		other.m_VertexStream = nullptr;
	}

	PolyMesh::~PolyMesh() {
		ZC_DEBUG("~PolyMesh()");
		delete m_VertexStream;
	}

	Vertex& PolyMesh::operator[] (int index) {
		return m_VertexStream[index];
	}

	Vertex* PolyMesh::GetVertexStream() {
		return m_VertexStream;
	}

	size_t PolyMesh::GetVertexCount() {
		return m_VertexCount;
	}

	void PolyMesh::SetVertexCount(size_t count) {
		m_VertexCount = count;
	}

	size_t PolyMesh::GetSizeInBytes() {
		return m_VertexCount * sizeof(Vertex);
	}
}