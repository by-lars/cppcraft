#include "Core/Base.h"
#include "Threading/ThreadPool.h"
#include "Threading/Mutex.h"
#include <Windows.h>

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
					ZC_DEBUG("Thread " << std::this_thread::get_id() << " is waiting");
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
				ZC_DEBUG("Thread " << std::this_thread::get_id() << " executing job " << job.JobFunction);
				job.JobFunction(job.Context);
			}
		}

		ZC_LOG("Thread shutting down");
	}

	ThreadPool::ThreadPool() {
		m_IsRunning = true;
	}

	void ThreadPool::Init() {
		int numThreads = 1;

		#ifdef ZC_PLATFORM_WIN32
		numThreads = std::thread::hardware_concurrency();
		#endif
		
		ZC_LOG("ThreadPool: Spawning " << numThreads << " threads");

		for (int i = 0; i < numThreads; i++) {
			HANDLE handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadProc, this, NULL, NULL);
			m_Threads.push_back(handle);
		}
	}

	ThreadPool::~ThreadPool() {
		ZC_DEBUG("ThreadPool: Shutting down threads...");

		m_JobMutex.Lock();
		m_IsRunning = false;
		m_JobMutex.Unlock();
		
		WaitForMultipleObjects(m_Threads.size(), &m_Threads[0], TRUE, INFINITE);
 	
		for (int i = 0; i < m_Threads.size(); i++) {
			CloseHandle(m_Threads[i]);
		}
	}

	void ThreadPool::SubmitWork(const Job& job) {
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