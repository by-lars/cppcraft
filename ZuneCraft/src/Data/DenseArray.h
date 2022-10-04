#pragma once
#include "Data/Handle.h"
#include <queue>
#include <vector>

namespace ZuneCraft {
	template<typename T>
	class DenseArray {
	public:
		DenseArray() { }

		Id PushBack(const T& object) {
			m_Handles.push_back(m_Objects.size());
			m_Objects.push_back(object);
			return Handle::Create<DenseArray<T>>(m_Handles.size() - 1);
		}

		T& Get(Id handle) {
			size_t index = Handle::Get<DenseArray<T>>(m_Handles, handle);
			return m_Objects[index];
		}

		void Erase(Id handle) {
			size_t indexToDelete = Handle::Get<DenseArray<T>>(m_Handles, handle);
			size_t lastIndex = m_Objects.size() - 1;

			m_Objects[indexToDelete] = m_Objects.back();
			m_Objects.pop_back();

			for (int i = 0; i < m_Handles.size(); i++) {
				if (m_Handles[i] == lastIndex) {
					m_Handles[i] = indexToDelete;
				}
			}
		}

		T& operator[](int index) {
			return m_Objects[index];
		}

		T* Data() {
			return &m_Objects[0];
		}

		size_t Size() {
			return m_Objects.size();
		}

	private:
		std::vector<size_t> m_Handles;
		std::vector<T> m_Objects;
	};
}