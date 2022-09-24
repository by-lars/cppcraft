#pragma once

#include <Windows.h>

namespace ZuneCraft {
	class Mutex {
	public:
		Mutex();
		~Mutex();

		void Lock();
		void Unlock();

	private:
		HANDLE m_Handle;
	};

	class ScopedLock {
	public:
		ScopedLock(Mutex& mutex);
		~ScopedLock();

	private:
		Mutex& m_Mutex;
	};
}