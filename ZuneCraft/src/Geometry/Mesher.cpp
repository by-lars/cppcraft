#include "Geometry/Mesher.h"
#include "Geometry/Material.h"
#include "Geometry/Vertex.h"

namespace ZuneCraft {
	void Mesher::VoxelToGreedy(const VoxelStorage& voxels, uint32_t lod, std::vector<Vertex>* _out_Mesh) {

		_out_Mesh->reserve(CHUNK_SIZE_QUBED);
		Face* mask = new Face[CHUNK_WIDTH * CHUNK_HEIGHT];
		int CHUNK_DIMENSIONS[3] = { CHUNK_WIDTH , CHUNK_HEIGHT , CHUNK_WIDTH  };

		for (int d = 0; d < 3; d++) {
			int w, h, k;

			int x[3] = { 0 };
			int q[3] = { 0 }; // (1, 0, 0); (0, 1, 0); (0, 0, 1);

			int u = (d + 1) % 3; // 1 2 0
			int v = (d + 2) % 3; // 2 0 1

			q[d] = 1;

			int normal = 2 * (d % 2) - 1;

			memset(mask, 0, CHUNK_WIDTH * CHUNK_HEIGHT);

			//Go through each slice of the chunk
			// (di, u, v); (v, di, u); (u, v, di);
			for (x[d] = -1; x[d] < CHUNK_DIMENSIONS[d];) {

				//Create the mask
				int n = 0;
				for (x[v] = 0; x[v] < CHUNK_DIMENSIONS[v]; x[v]++) {
					for (x[u] = 0; x[u] < CHUNK_DIMENSIONS[u]; x[u]++) {
						BlockType voxel = (x[d] >= 0) ? (BlockType)voxels.Data[x[0]][x[1]][x[2]] : BlockType::AIR;
						BlockType nextVoxel = (x[d] < CHUNK_DIMENSIONS[d] - 1) ? (BlockType)voxels.Data[x[0] + q[0]][x[1] + q[1]][x[2] + q[2]] : BlockType::AIR;

						bool isVoxelSolid = (voxel == BlockType::AIR);
						bool isNextVoxelSolid = (nextVoxel == BlockType::AIR);
						bool isBackFace = (isNextVoxelSolid == false);
					//	bool isBackFace = x[v] % 2;
						BlockType type = isNextVoxelSolid ? voxel : nextVoxel;

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
								_out_Mesh->push_back(Vertex(x[0] + du[0],			x[1] + du[1],			x[2] + du[2],			w, h, (uint8_t)mask[n].Type,	normal)); //Top Right
								_out_Mesh->push_back( Vertex(x[0],					x[1],					x[2],					0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
								_out_Mesh->push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1],	x[2] + du[2] + dv[2],	w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								_out_Mesh->push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1],	x[2] + du[2] + dv[2],	w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								_out_Mesh->push_back( Vertex(x[0],					x[1],					x[2],					0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
								_out_Mesh->push_back( Vertex(x[0] + dv[0],			x[1] + dv[1],			x[2] + dv[2],			0, 0, (uint8_t)mask[n].Type, normal)); //Lower left cornor
							}
							else {
								_out_Mesh->push_back( Vertex(x[0] + du[0], x[1] + du[1], x[2] + du[2], w, h, (uint8_t)mask[n].Type, normal)); //Top Right
								_out_Mesh->push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2], w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								_out_Mesh->push_back( Vertex(x[0], x[1], x[2], 0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
								_out_Mesh->push_back( Vertex(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2], w, 0, (uint8_t)mask[n].Type, normal)); //Lower right cornor
								_out_Mesh->push_back( Vertex(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2], 0, 0, (uint8_t)mask[n].Type, normal)); //Lower left cornor
								_out_Mesh->push_back( Vertex(x[0], x[1], x[2], 0, h, (uint8_t)mask[n].Type, normal)); //Top Left Cornor
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
	} 
}