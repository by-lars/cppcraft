#pragma once
#include "Core/Base.h"
#include <vector>

namespace ZuneCraft {
	typedef uint64_t Id;

	namespace Handle {
		typedef uint32_t HIndex;
		typedef uint32_t HType;

		namespace detail {
			extern HType s_HandleCounter;
		}

		const Id INVALID = ((Id)UINT32_MAX << 32);

		template <class T>
		HType GetTypeId() {
			static HType s_HandleTypeId = detail::s_HandleCounter++;
			return s_HandleTypeId;
		}

		template <class T>
		Id Create(HIndex index) {
			HType type = GetTypeId<T>();
			return ((Id)index << 32) | ((Id)type);
		}

		template<class T>
		bool IsOfType(Id handle) {
			HType otherType = GetTypeId<T>();
			HType type = (HType)handle;
			return otherType == type;
		}

		inline HIndex GetIndex(Id handle) {
			return (handle >> 32);
		}

		inline bool IsValid(Id handle) {
			return GetIndex(handle) != UINT32_MAX;
		}

		template<class T, class S>
		S& Get(std::vector<S>& vec, Id handle) {
			ZC_ASSERT(Handle::IsValid(handle), "Invalid Handle");
			ZC_ASSERT(Handle::IsOfType<T>(handle), "Unexpected handle type");
			size_t index = Handle::GetIndex(handle);
			ZC_ASSERT(index >= 0 && index < vec.size(), "Invalid handle");
			return vec[index];
		}
	}
}