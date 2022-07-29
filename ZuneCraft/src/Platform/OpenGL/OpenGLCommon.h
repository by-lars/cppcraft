#pragma once
#include "Core/Base.h"

namespace ZuneCraft {
	struct GLShader {
		uint32_t Id;
	};

	struct GLBuffer {
		uint32_t Id;
		uint32_t VertexArray;
		uint32_t Type;
		uint32_t AttributeCount;
	};

	struct GLRenderTarget {
		uint32_t FrameBuffer;
	};

	struct GLTexture {
		uint32_t Id;
		uint32_t Width;
		uint32_t Height;
		GLenum Format;
		GLenum DataType;
	};
}