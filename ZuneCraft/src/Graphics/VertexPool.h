#pragma once
#include "Graphics/RenderAPI.h"
#include "Graphics/DeviceResources.h"
#include "Data/Handle.h"
#include "Utility/Convert.h"
#include <vector>
#include <queue>
#include <imgui/imgui.h>


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

		void DebugDraw() {
			ImGui::Begin("VertexPool Info");
			ImGui::Text("Size: %i", m_MaxCount);
		
			int width = ImGui::GetWindowSize().x;

			ImGui::GetWindowWidth(); 
			VPNode* node = m_pLayout;
			int counter = 0;
			srand(0);
			ImGui::SetCursorPosY(100);

			ImDrawList* drawList = ImGui::GetWindowDrawList();

			while (node != nullptr) {
				ImGui::Text("Node #%i: Start: %i Length: %i IsFree: %i", counter, node->Start, node->GetSize(), node->IsFree);

				int eStart = ((float)node->Start / (float)m_MaxCount) * (float)width;
				int eWidth = ((float)node->GetSize() / (float)m_MaxCount) * (float)width;
				eWidth += eStart;
				

				ImVec2 min(eStart, 45); 
				min.x += ImGui::GetWindowPos().x;
				min.y += ImGui::GetWindowPos().y;

				ImVec2 max(eWidth, 64);
				max.x += ImGui::GetWindowPos().x;
				max.y += ImGui::GetWindowPos().y;


				if (node->IsFree) {
					drawList->AddRect(min, max, IM_COL32(0, 255, 0, 255), 0, 0, 2.0f);
				}
				else {
					drawList->AddRectFilled(min, max, IM_COL32(rand() % 255, 0, rand() % 255, 255));
				}
			
				std::string text = "#" + ToString(counter);
				drawList->AddText(min, IM_COL32(255, 255, 255, 255), text.c_str());
				
				node = node->Next;
				counter++;
			}
			ImGui::End();
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
