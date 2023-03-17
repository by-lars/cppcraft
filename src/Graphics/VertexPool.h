#pragma once
#include "Graphics/RenderAPI.h"
#include "Graphics/DeviceResources.h"
#include "Data/Handle.h"
#include "Utility/Convert.h"
#include <vector>
#include <queue>

namespace ZuneCraft {
	struct VPNode {
		size_t Start;
		size_t End;

		inline size_t GetSize() { return End - Start; }

		VPNode* Next;
		bool IsFree;
		uint32_t BatchId;
	};

	class VertexPool {
	public:
		VertexPool(Pipeline* pipeline, Ref<GPUStorage> storage);
		~VertexPool();
		VPNode* PushBack(void* meshData, size_t meshSize, size_t batchId);
		void Free(VPNode* node);
		void Cleanup();
		void GenerateRenderCommands();

	private:
		Pipeline* m_Pipeline;
		Ref<GPUStorage> m_Storage;
		VPNode* m_pLayout;
		bool m_Invalidated;
	};
}
