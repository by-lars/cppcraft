#pragma once
#include "Core/Base.h"
#include "Utility/Handle.h"
#include <vector>

namespace ZuneCraft {
	ZC_ENUM DrawMode {
		TRIANGLES = 0,
		TRIANGLE_STRIP,
		LINES
	};

	ZC_ENUM StorageType {
		VERTEX = 0,
		BATCH,
		SHADER,
		DRAW_COMMAND
	};

	ZC_ENUM StorageUsage {
		STATIC = 0,
		DYNAMIC,
		STREAM
	};

	ZC_ENUM StorageFormat{
		UBYTE_VEC4 = 0,
		UBYTE_VEC4_VEC4,

		FLOAT_VEC4,
		FLOAT_VEC3,

		INT_VEC1
	};

	ZC_ENUM DataType{
		INT = 0,
		FLOAT,
		UNSIGNED_BYTE
	};


	ZC_ENUM ClampMode {
		REPEAT = 0,
		CLAMP_TO_EDGE,
	};

	ZC_ENUM FilterMode {
		NEAREST = 0,
		LINEAR
	};

	ZC_ENUM TextureFormat {
		RGB = 0,
		RGBA,

		DEPTH_COMPONENT16, 
		DEPTH_COMPONENT24, 
		DEPTH_COMPONENT32
	};

	struct RenderCommand {
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t First;
		uint32_t BaseInstance;
	};

	struct BufferSpec {
		uint32_t Count;
		StorageType Type;
		StorageUsage Usage;
		StorageFormat Format;
		Id ParrentBuffer;
		Id Shader;
	};

	struct TextureSpec {
		uint32_t Width;
		uint32_t Height;
		TextureFormat Format;
		DataType DataType;
		ClampMode ClampMode;
		FilterMode FilterMode;
	};

	ZC_ENUM AttachementType {
		Color = 0,
		Depth
	};

	struct FramebufferAttachement {
		FramebufferAttachement(TextureFormat format, AttachementType type, bool writeOnly);
		TextureFormat Format;
		AttachementType Type;
		bool WriteOnly;
	};

	struct FramebufferSpec {
		uint32_t Width;
		uint32_t Height;
		std::vector<FramebufferAttachement> Attachements;
	};
}