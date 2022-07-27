#pragma once

#include "Core/Base.h"
#include "Geometry/VoxelStorage.h"
#include "Geometry/Vertex.h"

namespace ZuneCraft {
	namespace Mesher {
		void VoxelToGreedy(const VoxelStorage& voxels, Vertex* _out_VertexData, size_t* _out_VertCount);

		const float CubeVertices[] = {
            -0.5f, -0.5f, -0.5f, // Bottom-left
             0.5f,  0.5f, -0.5f, // top-right
             0.5f, -0.5f, -0.5f, // bottom-right         
             0.5f,  0.5f, -0.5f, // top-right
            -0.5f, -0.5f, -0.5f, // bottom-left
            -0.5f,  0.5f, -0.5f, // top-left
            // Front face
            -0.5f, -0.5f,  0.5f, // bottom-left
             0.5f, -0.5f,  0.5f, // bottom-right
             0.5f,  0.5f,  0.5f, // top-right
             0.5f,  0.5f,  0.5f, // top-right
            -0.5f,  0.5f,  0.5f, // top-left
            -0.5f, -0.5f,  0.5f, // bottom-left
            // Left face
            -0.5f,  0.5f,  0.5f, // top-right
            -0.5f,  0.5f, -0.5f, // top-left
            -0.5f, -0.5f, -0.5f, // bottom-left
            -0.5f, -0.5f, -0.5f, // bottom-left
            -0.5f, -0.5f,  0.5f, // bottom-right
            -0.5f,  0.5f,  0.5f, // top-right
            // Right face
             0.5f,  0.5f,  0.5f, // top-left
             0.5f, -0.5f, -0.5f, // bottom-right
             0.5f,  0.5f, -0.5f, // top-right         
             0.5f, -0.5f, -0.5f, // bottom-right
             0.5f,  0.5f,  0.5f, // top-left
             0.5f, -0.5f,  0.5f, // bottom-left     
            // Bottom face
            -0.5f, -0.5f, -0.5f, // top-right
             0.5f, -0.5f, -0.5f, // top-left
             0.5f, -0.5f,  0.5f, // bottom-left
             0.5f, -0.5f,  0.5f, // bottom-left
            -0.5f, -0.5f,  0.5f, // bottom-right
            -0.5f, -0.5f, -0.5f, // top-right
            // Top face
            -0.5f,  0.5f, -0.5f, // top-left
             0.5f,  0.5f,  0.5f, // bottom-right
             0.5f,  0.5f, -0.5f, // top-right     
             0.5f,  0.5f,  0.5f, // bottom-right
            -0.5f,  0.5f, -0.5f, // top-left
            -0.5f,  0.5f,  0.5f // bottom-left   
		};

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
	};
}