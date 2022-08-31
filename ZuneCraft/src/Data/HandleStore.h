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

		HandleStore() = default;

		Id PushBack(const T& object) {
			if (m_FreeHandles.empty()) {
				
				Id handle = Handle::Create<HandleStore<T>>(m_Objects.size());

				m_Objects.push_back(Entry{
					.Handle = handle,
					.Object = object,
					.IsActive = true
				});

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

	private:
		std::queue<Id> m_FreeHandles;
		std::vector<Entry> m_Objects;
	};
}