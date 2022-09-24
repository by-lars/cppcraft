#pragma once
#include "Graphics/RenderAPI.h"
#include "Graphics/DeviceResources.h"
#include "Data/Handle.h"
#include "Utility/Convert.h"
#include <vector>
#include <queue>
//#include <imgui/imgui.h>


namespace ZuneCraft {
	struct VPNode {
		size_t Start;
		size_t End;
		Id RenderCommand;
		inline size_t GetSize() { return End - Start; }

		VPNode* Next;
		bool IsFree;
	};

	template<typename MeshType>
	class VertexPool {
	public:
		VertexPool(RenderAPI* device, StorageFormat format, uint32_t maxCount) : m_Device(device), m_MaxCount(maxCount) {
			BufferSpec spec;
			spec.Count = maxCount;
			spec.Usage = StorageUsage::DYNAMIC;
			spec.Type = StorageType::VERTEX;
			spec.Format = format;
			spec.ParrentBuffer = Handle::INVALID;
			m_Buffer = m_Device->StorageCreate(spec);

			m_pLayout = new VPNode();
			m_pLayout->Start = 0;
			m_pLayout->End = maxCount;
			m_pLayout->IsFree = true;
			m_pLayout->Next = nullptr;
		}

		~VertexPool() {
			VPNode* node = m_pLayout;
			while (node != nullptr) {
				VPNode* tmp = node;
				node = tmp->Next;
				delete tmp;
			}
		}
		
		VPNode* PushBack(std::vector<MeshType>& mesh, size_t batchId) {
			if (mesh.size() < 1) { ZC_WARN("Can't push empty mesh"); return nullptr; }

			VPNode* node = m_pLayout;
			bool found = false;
			size_t size = mesh.size();

			while (node != nullptr) {
				if (node->IsFree && node->GetSize() >= size) {
					found = true;
					break;
				}

				node = node->Next;
			}
			
			if (!found) {
				ZC_WARN("OUT OF MEM");
				return nullptr;
			}
			
			m_Device->StorageUpload(m_Buffer, size, node->Start, &mesh[0]);
			
			size_t remaingStorage = node->GetSize() - size;

			node->End = node->Start + size;
			node->IsFree = false;

			VPNode* newNode = new VPNode();
			newNode->Start = node->End;
			newNode->End = newNode->Start + remaingStorage;
			newNode->IsFree = true;

			newNode->Next = node->Next;
			node->Next = newNode;

			RenderCommand cmd;
			cmd.Count = node->GetSize();
			cmd.InstanceCount = 1;
			cmd.First = node->Start;
			cmd.BaseInstance = batchId;
			node->RenderCommand = m_Device->RenderCommandPush(cmd);

			return node;
		}
		
		void Free(VPNode* node) {
			if (node == nullptr) {
				ZC_WARN("Tried to free mesh with VPNode* = nullptr");
				return;
			}

			node->IsFree = true;
			m_Device->RenderCommandErase(node->RenderCommand);
		}

		Id GetBufferId() {
			return m_Buffer;
		}

		void Cleanup() {
			VPNode* node = m_pLayout;

			while (node != nullptr) {
				if (node->Next == nullptr)
					break;

				if (node->IsFree && node->Next->IsFree) {
					node->End = node->Next->End;

					VPNode* nextNode = node->Next->Next;
					delete node->Next;

					node->Next = nextNode;
				}


				node = node->Next;
			}
		}

	private:
		RenderAPI* m_Device;
		Id m_Buffer;

		VPNode* m_pLayout;

		uint32_t m_MaxCount;
	};
}
