#pragma once
#include <Core/Base.h>


#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 128

#define CHUNK_SIZE_SQUARED CHUNK_WIDTH * CHUNK_WIDTH
#define CHUNK_SIZE_QUBED CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT

namespace ZuneCraft {
	//const int CHUNK_DIMENSIONS[3] = { CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH };

	struct VoxelStorage {
		uint8_t Data[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];
	};
}