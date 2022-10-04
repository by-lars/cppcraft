#pragma once
#include "Core/Base.h"
#include "Data/Handle.h"
#include <vector>
#include <glm/glm.hpp>

namespace ZuneCraft {

	ZC_ENUM StorageUsage{
		MESH_STATIC = 0,
		MESH_DYNAMIC,
		INSTANCE_DATA,
		DRAW_COMMAND
	};

	ZC_ENUM DrawMode{
		TRIANGLES = 0,
		TRIANGLE_STRIP,
		LINES
	};

	ZC_ENUM StorageFormat{
		UBYTE_VEC4 = 0,
		UBYTE_VEC4_VEC4,

		FLOAT_VEC4,
		FLOAT_VEC3,
		FLOAT_VEC2,

		INT_VEC1,

		DRAW_COMMAND
	};

	ZC_ENUM FilterMode {
		NEAREST = 0,
		LINEAR
	};

	ZC_ENUM TextureFormat{
		RGB = 0,
		RGBA,

		DEPTH_COMPONENT16,
		DEPTH_COMPONENT24,
		DEPTH_COMPONENT32
	};

	ZC_ENUM UsageHint {
		DYNAMIC = 0,
		STATIC = 1
	};

	struct RenderCommand {
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t First;
		uint32_t BaseInstance;
	};

	class Texture {
	public:
		virtual ~Texture() { }
		virtual void Bind() = 0;
		virtual void Upload(void* data) = 0;
		virtual uint32_t GetNativeHandle() = 0;
	};

	class Shader {
	public:
		virtual ~Shader() { }
		virtual void Bind() = 0;
		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, const glm::vec3& value) = 0;
		virtual void Set(const std::string& name, const glm::vec4& value) = 0;
		virtual void Set(const std::string& name, const glm::mat4& value) = 0;
		virtual uint32_t GetNativeHandle() = 0;
	};

	class RenderTarget {
	public:
		virtual ~RenderTarget() { }
		virtual void AddTexture(TextureFormat format) = 0;
		virtual void AddRenderBuffer(TextureFormat format) = 0;
		virtual void Finalize() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Bind() = 0;
		virtual uint32_t GetNativeHandle() = 0;
	};

	class GPUStorage {
	public:
		virtual ~GPUStorage() {}
		virtual void Upload(uint32_t size, uint32_t offset, void* data) = 0;
		virtual void Bind() = 0;
		virtual uint32_t GetNativeHandle() = 0;
	};

	class VertexLayout {
	public:
		virtual ~VertexLayout() { }
		virtual void AddFormat(GPUStorage* storage, StorageFormat format, StorageUsage usage) = 0;
		virtual void Bind() = 0;
	};

	class CommandQueue {
	public:
		virtual ~CommandQueue() { }
		virtual void Push(const RenderCommand& cmd) = 0;
		virtual void Clear() = 0;
		virtual void Draw() = 0;
	};
}