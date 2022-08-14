#include "Geometry/Vertex.h"
#include "Graphics/GL.h"

namespace ZuneCraft {
	Vertex::Vertex() {
		X = 0; Y = 0; Z = 0; Direction = 0; U = 0; V = 0; Type = 0; BatchIndex = 0;
	}

	Vertex::Vertex(uint8_t _x, uint8_t _y, uint8_t _z, uint8_t _u, uint8_t _v, uint8_t _type, uint8_t _direction) {
		X = _x; Y = _y; Z = _z;
		U = _u;
		V = _v;
		Type = _type;
		Direction = _direction;
		BatchIndex = 0;
	}

	bool Face::operator==(const Face& other) {
		return Type == other.Type;
	}

	bool Face::operator!=(const Face& other) {
		return Type != other.Type;
	}
}