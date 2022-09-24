#include "Core/Base.h"
#include "Threading/Mutex.h"
#include <Windows.h>

namespace ZuneCraft {
	Mutex::Mutex() {
		m_Handle = CreateMutex(NULL, FALSE, NULL);
		if (m_Handle == NULL) {
			ZC_FATAL_ERROR("Could not create mutex. GetLastError(): " << GetLastError()); 
		}
	}

	Mutex::~Mutex() {
		CloseHandle(m_Handle);
	}

	void Mutex::Lock() {
		DWORD result = WaitForSingleObject(m_Handle, INFINITE);
		
		if (result == WAIT_OBJECT_0) { return; }
		if (result == WAIT_ABANDONED) { ZC_FATAL_ERROR("Mutex was abandonned"); }
	}

	void Mutex::Unlock() {
		if (!ReleaseMutex(m_Handle)) {
			ZC_FATAL_ERROR("Could not release ownership of mutex. GetLastError(): " << GetLastError());
		}
	}

	ScopedLock::ScopedLock(Mutex& mutex)
		: m_Mutex(mutex) {
		m_Mutex.Lock();
	}

	ScopedLock::~ScopedLock() {
		m_Mutex.Unlock();
	}
}