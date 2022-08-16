#pragma once
#include "Core/Base.h"


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
	}
}