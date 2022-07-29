#pragma once

#include "Core/Base.h"
#include "Geometry/VoxelStorage.h"
#include "Geometry/Vertex.h"

namespace ZuneCraft {
    class PolyMesh {
    public:
        PolyMesh(PolyMesh&& other);
        PolyMesh(size_t vertexCount);
        ~PolyMesh();

        Vertex& operator[] (int index);

        Vertex* GetVertexStream();
        size_t GetVertexCount();
        size_t GetSizeInBytes();

        void SetVertexCount(size_t count);

    private:
        Vertex* m_VertexStream;
        size_t m_VertexCount;
    };
    
    namespace Mesher {
		PolyMesh VoxelToGreedy(const VoxelStorage& voxels);

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