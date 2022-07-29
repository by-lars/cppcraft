#pragma once
#include "Core/Base.h"

#include <unordered_map>
#include <array>

namespace ZuneCraft {
	template <typename T, size_t S>
	class SparseArray {
	public:
		SparseArray() {
			m_Dense = { };
			m_Size = 0;
			m_IndexCounter = 0;
		}

		size_t PushBack(const T& data) {
			m_Dense[m_Size] = data;
			m_SparseToDense[m_IndexCounter] = m_Size;
			m_DenseToSparse[m_Size] = m_IndexCounter;
			m_Size++;
			m_IndexCounter++;

			return m_IndexCounter - 1;
		}

		void Erase(size_t indexToErase) {
			ZC_ASSERT(m_SparseToDense.find(indexToErase) != m_SparseToDense.end() , "Element is not is sparse array");

			size_t lastIndex = m_Size - 1;
			size_t removedIndex = m_SparseToDense[indexToErase];
			size_t movedEntity = m_DenseToSparse[lastIndex];

			//Swap last element in dense array with deleted element
			m_Dense[removedIndex] = m_Dense[lastIndex];

			//Update mapping
			m_SparseToDense[movedEntity] = removedIndex;
			m_DenseToSparse[removedIndex] = movedEntity;

			//Cleanup 
			m_SparseToDense.erase(indexToErase);
			m_DenseToSparse.erase(lastIndex);

			m_Size--;
		}

		void Clear() {
			m_DenseToSparse.clear();
			m_SparseToDense.clear();
			m_Dense = {};
			m_Size = 0;
		}

		T& Get(size_t index) {
			ZC_ASSERT(m_SparseToDense.find(index) != m_SparseToDense.end() , "Element is not in sparse array");
			return m_Dense[m_SparseToDense[index]];
		}

		void Set(size_t index, const T& data) {
			ZC_ASSERT(m_SparseToDense.find(index) != m_SparseToDense.end() , "Element is not in sparse array");
			m_Dense[m_SparseToDense[index]] = data;
		}

		T* DenseData() { return m_Dense.data(); }

		size_t Count() { return m_Size; }

	private:
		std::array<T, S> m_Dense;
		std::unordered_map<size_t, size_t> m_SparseToDense;
		std::unordered_map<size_t, size_t> m_DenseToSparse;
		std::vector<size_t> m_FreeSlots;
		size_t m_Size;
		size_t m_IndexCounter;
	};
}