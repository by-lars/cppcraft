#include "Core/Base.h"
#include "Threading/Signal.h"
#include <windows.h>

namespace ZuneCraft {
	Signal::Signal() {
		m_Handle = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_Handle == NULL) { 
			ZC_FATAL_ERROR("Could not create event object. GetLastError(): " << GetLastError()); 
		}
	}

	Signal::~Signal() {
		CloseHandle(m_Handle);
	}

	void Signal::Wait() {
		DWORD result = WaitForSingleObject(m_Handle, INFINITE);

		if (result != WAIT_OBJECT_0) { 
			ZC_FATAL_ERROR("Error while waiting for event. GetLastError(): " << GetLastError()); 
		}
	}

	void Signal::SignalOne() {
		if (!SetEvent(m_Handle)) {
			ZC_FATAL_ERROR("Could not signal event. GetLastError(): " << GetLastError());
		}
	}
}