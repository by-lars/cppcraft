#include "Graphics/VertexPool.h"
#include <functional>

namespace ZuneCraft {
	VertexPool::VertexPool(Pipeline* pipeline, Ref<GPUStorage> storage)
		: m_Pipeline(pipeline), m_Storage(storage), m_Invalidated(false) {
	
		m_pLayout = new VPNode();
		m_pLayout->Start = 0;
		m_pLayout->End = m_Storage->GetMaxCount();
		m_pLayout->IsFree = true;
		m_pLayout->Next = nullptr;
	}

	VertexPool::~VertexPool() {
		VPNode* node = m_pLayout;
		while (node != nullptr) {
			VPNode* tmp = node;
			node = tmp->Next;
			delete tmp;
		}
	}

	VPNode* VertexPool::PushBack(void* meshData, size_t meshSize, size_t batchId) {
		if (meshSize < 1) { ZC_WARN("Can't push empty mesh"); return nullptr; }

		VPNode* node = m_pLayout;
		bool found = false;

		while (found == false) {
			if (node->IsFree && node->GetSize() >= meshSize) {
				found = true;
				break;
			}

			if (node->Next == nullptr) {
				break;
			}

			node = node->Next;
		}

		if (!found) {
			ZC_WARN("VertexPool: Out of mem, allocating more...");
			VPNode* newNode = new VPNode();
			newNode->Start = node->End;
			
			uint32_t newSize = (m_Storage->GetMaxCount()+meshSize) * 1.75f;
			m_Storage->Resize(newSize);
			newNode->End = newSize;
			newNode->IsFree = true;
			node->Next = newNode;

			node = newNode;
		}

		m_Storage->Upload(meshSize, node->Start, meshData);

		size_t remaingStorage = node->GetSize() - meshSize;

		node->BatchId = batchId;
		node->End = node->Start + meshSize;
		node->IsFree = false;

		VPNode* newNode = new VPNode();
		newNode->Start = node->End;
		newNode->End = newNode->Start + remaingStorage;
		newNode->IsFree = true;

		newNode->Next = node->Next;
		node->Next = newNode;

		m_Invalidated = true;

		return node;
	}

	void VertexPool::Free(VPNode* node) {
		if (node == nullptr) {
			ZC_WARN("Tried to free mesh with VPNode* = nullptr");
			return;
		}

		node->IsFree = true;
		m_Invalidated = true;
	}


	void VertexPool::Cleanup() {
		VPNode* node = m_pLayout;

		while (node != nullptr) {
			if (node->Next == nullptr)
				break;

			if (node->IsFree && node->Next->IsFree) {
				node->End = node->Next->End;

				VPNode* nextNode = node->Next->Next;
				delete node->Next;
				m_Invalidated = true;
				node->Next = nextNode;
			}


			node = node->Next;
		}

	}

	void VertexPool::GenerateRenderCommands() {
		//TODO: Add visibility checking
		//TODO: Merge RenderCommands when possible

		if (!m_Invalidated)
			return;

		m_Pipeline->ClearCommands();

		//int counter = 0;

		VPNode* node = m_pLayout;
		while (node != nullptr) {
			if (node->Next == nullptr)
				break;

			if (node->IsFree == false) {
				RenderCommand cmd { };
				cmd.Count = node->GetSize();
				cmd.InstanceCount = 1;
				cmd.First = node->Start;
				cmd.BaseInstance = node->BatchId;
				m_Pipeline->PushCommand(cmd);
		//		counter++;
			}

			node = node->Next;
		}

		//ZC_DEBUG(counter);
		m_Invalidated = false;
	}
}