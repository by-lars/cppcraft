#pragma once

#include "Utility/StrongHandle.h"
#include <stdint.h>

namespace ZuneCraft {
	ZC_MAKE_STRONG_HANDLE(HShader, int, -1);
	ZC_MAKE_STRONG_HANDLE(HBuffer, int, -1);
	ZC_MAKE_STRONG_HANDLE(HTexture, int, -1);
	ZC_MAKE_STRONG_HANDLE(HRenderTarget, int, -1);

	enum class DrawMode {
		TRIANGLES = 0,
		TRIANGLE_STRIP,
		LINES
	};

	enum class BufferType {
		ARRAY = 0
	};

	enum class BufferUsage {
		STATIC_DRAW = 0,
		STATIC_READ,
		STATIC_COPY,

		DYNAMIC_DRAW,
		DYNAMIC_READ,
		DYNAMIC_COPY,

		STREAM_DRAW,
		STREAM_READ,
		STREAM_COPY
	};

	enum class DataType {
		INT = 0,
		FLOAT,
		UNSIGNED_BYTE
	};

	struct BufferElement {
		DataType Type;
		uint32_t Count;
		uint32_t Divisor;

		uint32_t GetSizeInBytes() const;
		bool IsIntegerType() const;
	};

	enum class ClampMode {
		REPEAT = 0,
		CLAMP_TO_EDGE,
	};

	enum class FilterMode {
		NEAREST,
		LINEAR
	};

	enum class TextureFormat {
		RGB,
		RGBA
	};
}