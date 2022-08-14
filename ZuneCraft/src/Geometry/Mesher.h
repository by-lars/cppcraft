#pragma once

#include "Core/Base.h"
#include "Geometry/VoxelStorage.h"
#include "Geometry/Vertex.h"

#include <vector>

namespace ZuneCraft {
    namespace Mesher {
		void VoxelToGreedy(const VoxelStorage& voxels, uint32_t lod, std::vector<Vertex>* _out_Mesh);

        const float LineCubeVertices[] = {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, 0.5f,
             0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, 0.5f, -0.5f,
             0.5f, 0.5f, -0.5f,
             0.5f, 0.5f, -0.5f,
             0.5f, 0.5f, 0.5f,
             0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f,

            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
        };

        #ifdef ZC_PLATFORM_ZUNE
        //Texture coords are rotated, to render in landscape on a portrait buffer
        const unsigned char FullscreenQuad[] = {
            0, 1, 0, 0,
            0, 0, 1, 0,
            1, 1, 0, 1,
            1, 0, 1, 1
        };
        #elif ZC_PLATFORM_WIN32
        const unsigned char FullscreenQuad[] = {
            0, 1, 0, 1,
            0, 0, 0, 0,
            1, 1, 1, 1,
            1, 0, 1, 0
        };
        #endif

	};
}