#pragma once
#include "Core/Base.h"
#include <vector>
#include <typeinfo>

namespace ZuneCraft {
	class IService {
	public:
		virtual ~IService() {}
	};

	class Services {
	public:
		template<class T>
		static T* Get() {
			return (T*)s_Services[GetServiceIndex<T>()];
		}

		template<class T>
		static T* Initialize() {
			ZC_DEBUG("Registering Service " << typeid(T).name());
			GetServiceIndex<T>();
			s_Services.push_back(new T());
			return (T*)s_Services.back();
		}

		static void Shutdown();

	private:
		template<class T>
		static size_t GetServiceIndex() {
			static size_t index = s_Services.size();
			return index;
		}

		static std::vector<IService*> s_Services;
	};
}