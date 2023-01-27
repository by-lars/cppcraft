#include "Core/Base.h"
#include "Threading/ThreadPool.h"
#include "Threading/Mutex.h"
#include <windows.h>

#ifdef ZC_PLATFORM_WIN32
#include <thread>
#endif


namespace ZuneCraft {
	void ThreadPool::ThreadProc(LPVOID instance) {
		ThreadPool* pool = (ThreadPool*)instance;
		
		Job job;
		while (true) {
			{
				ScopedLock lock(pool->m_JobMutex);

				if (job.CallbackFunction != nullptr) {
					pool->m_PendingCallbacks.push_back(job);
				}

				job.CallbackFunction = nullptr;
				job.JobFunction = nullptr;

				if (pool->m_Jobs.empty()) {
					pool->m_JobMutex.Unlock();
					pool->m_JobSignal.Wait();
					pool->m_JobMutex.Lock();
				}

				if (!pool->m_IsRunning) {
					break;
				}

				if (!pool->m_Jobs.empty()) {
					job = pool->m_Jobs.back();
					pool->m_Jobs.pop_back();
				}
			}
			
			if (job.JobFunction != nullptr) {
				job.JobFunction(job.Context);
			}
		}

		ZC_LOG("Thread shutting down");
	}

	ThreadPool::ThreadPool() {
		m_IsRunning = true;
	
		int numThreads = 1;

		#ifdef ZC_PLATFORM_WIN32
			HANDLE currentThread = GetCurrentThread();
			SetThreadPriority(currentThread, THREAD_PRIORITY_ABOVE_NORMAL);
			numThreads = std::thread::hardware_concurrency();
		#endif

		ZC_LOG("ThreadPool: Spawning " << numThreads << " threads");

		for (int i = 0; i < numThreads; i++) {
			HANDLE handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadProc, this, NULL, NULL);

			if (handle == NULL) {
				ZC_FATAL_ERROR("Could not create thread");
			}

			SetThreadPriority(handle, THREAD_PRIORITY_LOWEST);
			m_Threads.push_back(handle);
		}
	}

	ThreadPool::~ThreadPool() {
		ZC_DEBUG("ThreadPool: Shutting down threads...");

		m_JobMutex.Lock();
		m_IsRunning = false;
		m_JobMutex.Unlock();
		
		for (int i = 0; i < m_Threads.size(); i++) {
			m_JobSignal.SignalOne();
		}

		WaitForMultipleObjects((DWORD)m_Threads.size(), &m_Threads[0], TRUE, INFINITE);
 	
		for (int i = 0; i < m_Threads.size(); i++) {
			CloseHandle(m_Threads[i]);
		}
	}

	void ThreadPool::SubmitWork(function_t jobFunction, function_t callbackFunction, void* context) {
		Job job;
		job.JobFunction = jobFunction;
		job.CallbackFunction = callbackFunction;
		job.Context = context;

		ScopedLock lock(m_JobMutex);
		m_Jobs.push_back(job);
		m_JobSignal.SignalOne();
	}

	void ThreadPool::RunCallbacks() {
		ScopedLock lock(m_JobMutex);

		for (int i = 0; i < m_PendingCallbacks.size(); i++) {
			Job& job = m_PendingCallbacks[i];
			job.CallbackFunction(job.Context);
		}

		m_PendingCallbacks.clear();
	}
}