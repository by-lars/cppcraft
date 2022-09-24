#include "ThreadPool.h"
#include <Windows.h>

namespace ZuneCraft {
	struct ThreadPoolData {

	};

	void ThreadProc(LPVOID parameter) {

	}

	void ThreadPool::Init() {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadProc, 0, 0, 0);
	}

	void ThreadPool::SubmitWork() {

	}

}