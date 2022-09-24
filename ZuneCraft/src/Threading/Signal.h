#pragma once
#include <Windows.h>

namespace ZuneCraft {
	class Signal {
	public:
		Signal();
		~Signal();

		void Wait();
		void SignalOne();

	private:
		HANDLE m_Handle;
	};
}