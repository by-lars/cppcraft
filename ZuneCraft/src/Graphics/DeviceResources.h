#pragma once

#include "Utility/StrongHandle.h"
#include "Core/Base.h"

namespace ZuneCraft {
	ZC_MAKE_STRONG_HANDLE(HShader, int, -1);
	ZC_MAKE_STRONG_HANDLE(HBuffer, int, -1);
	ZC_MAKE_STRONG_HANDLE(HTexture, int, -1);
	ZC_MAKE_STRONG_HANDLE(HRenderTarget, int, -1);

	ZC_ENUM DrawMode {
		TRIANGLES = 0,
		TRIANGLE_STRIP,
		LINES
	};

	ZC_ENUM BufferType {
		ARRAY = 0,
		DRAW_INDIRECT_BUFFER
	};

	ZC_ENUM BufferUsage {
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

	ZC_ENUM DataType {
		INT = 0,
		FLOAT,
		UNSIGNED_BYTE
	};

	struct BufferElement {
		BufferElement(DataType type, uint32_t count, uint32_t divisor);
		DataType Type;
		uint32_t Count;
		uint32_t Divisor;

		uint32_t GetSizeInBytes() const;
		bool IsIntegerType() const;
	};

	ZC_ENUM ClampMode {
		REPEAT = 0,
		CLAMP_TO_EDGE,
	};

	ZC_ENUM FilterMode {
		NEAREST,
		LINEAR
	};

	ZC_ENUM TextureFormat {
		RGB,
		RGBA
	};

	struct RenderCommand {
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t First;
		uint32_t BaseInstance;
	};
}