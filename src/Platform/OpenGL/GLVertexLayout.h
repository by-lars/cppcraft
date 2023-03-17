#pragma once
#include "Core/Base.h"
#include "Graphics/DeviceResources.h"

namespace ZuneCraft {
	class GLVertexLayout : public VertexLayout {
	public:
		GLVertexLayout();
		void AddFormat(Ref<GPUStorage>& storage, StorageFormat format, StorageUsage usage) override;
		void Bind() override;

	private:
		GLuint m_Handle;
		GLuint m_AttribCounter;
	};

	class GLES2VertexLayout : public VertexLayout {
	public:
		GLES2VertexLayout();
		void AddFormat(Ref<GPUStorage>& storage, StorageFormat format, StorageUsage usage) override;
		void Bind() override;

	private:
		struct Layout {
			Ref<GPUStorage> Storage;
			GLuint Count;
			GLenum Type;
			GLuint Stride;
			GLuint Offset;
		};

		std::vector<Layout> m_Layout;
	};
}