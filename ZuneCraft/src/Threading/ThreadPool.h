#pragma once

#include "Core/Service.h"

namespace ZuneCraft {
	class ThreadPool : public IService {
	public:
		void Init() override;

		void SubmitWork();
	
	private:
		
	};
}