#pragma once

#include "Core/Base.h"
#include "Geometry/Material.h"

namespace ZuneCraft {
	struct Face {
		Face() : IsSolid(false), IsBackFace(false), Type(BlockType::AIR), Direction(0) {}
		Face(bool isSolid, bool isBackFace, BlockType type, uint8_t direction) : IsSolid(isSolid), IsBackFace(isBackFace), Type(type), Direction(direction) {}
		bool IsSolid;
		bool IsBackFace;
		BlockType Type;
		uint8_t Direction;
	};

	struct Vertex {
		Vertex();
		Vertex(uint8_t _x, uint8_t _y, uint8_t _z, uint8_t _u, uint8_t _v, uint8_t type, uint8_t _direction);
		uint8_t X;
		uint8_t Y;
		uint8_t Z;
		uint8_t Type;
		uint8_t U;
		uint8_t V;
		uint8_t Direction;
		uint8_t BatchIndex;
	};
}