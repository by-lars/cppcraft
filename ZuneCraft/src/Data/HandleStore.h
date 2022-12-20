#pragma once
#include <vector>
#include <queue>
#include "Data/Handle.h"

namespace ZuneCraft {
	template<typename T>
	class HandleStore {
	public:
		struct Entry {
			Id Handle;
			T Object;
			bool IsActive;
		};

		HandleStore() { }

		Id PushBack(const T& object) {
			if (m_FreeHandles.empty()) {
				
				Id handle = Handle::Create<HandleStore<T>>((Handle::HIndex)m_Objects.size());

				Entry entry { handle , object, true };
				m_Objects.push_back(entry);

				return handle;
			}

			Id handle = m_FreeHandles.front();
			m_FreeHandles.pop();

			Entry& entry = Handle::Get<HandleStore<T>>(m_Objects, handle);
			entry.Object = object;
			entry.Handle = handle;
			entry.IsActive = true;

			return handle;
		}

		T& Get(Id handle) {
			Entry& entry = Handle::Get<HandleStore<T>>(m_Objects, handle);
			ZC_ASSERT(entry.IsActive, "Can't retrieve deleted element");

			return entry.Object;
		}

		void Erase(Id handle) {
			Entry& entry = Handle::Get<HandleStore<T>>(m_Objects, handle);
			entry.IsActive = false;
			m_FreeHandles.push(entry.Handle);
		}

		T& operator[] (Id handle) {
			return Get(handle);
		}

	private:
		std::queue<Id> m_FreeHandles;
		std::vector<Entry> m_Objects;
	};
}