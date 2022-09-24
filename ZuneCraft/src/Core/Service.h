#pragma once
#include "Core/Base.h"
#include <vector>

namespace ZuneCraft {
	class IService {
	public:
		virtual void Init() = 0;
		virtual ~IService() {}
	};

	class Services {
	public:
		template<class T>
		static T* Get() {
			return (T*)s_Services[GetServiceIndex<T>()];
		}

		template<class T>
		static void Register() {
			ZC_DEBUG("Registering Service " << typeid(T).name());
			GetServiceIndex<T>();
			s_Services.push_back(new T());
		}

		static void Initialize();
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