#include "Core/Base.h"
#include "Threading/ThreadPool.h"

namespace ZuneCraft {
	void ThreadPool::ThreadProc() {
		while (true) {
			JobData job;
			{
				std::unique_lock<std::mutex> lock(m_JobQueueMutex);

				m_ThreadSignal.wait(lock, [this] {
					return !m_Jobs.empty() || m_IsRunning == false;
				});

				if(m_IsRunning == false) {
					return;
				}

				job = m_Jobs.front();
				m_Jobs.pop();
			}

			job.job(job.context);

			{
				std::unique_lock<std::mutex> lock(m_CallbackQueueMutex);
				m_PendingCallbacks.push_back(job);
			}
		}
	}

	ThreadPool::ThreadPool() {
		for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++) {
			m_Threads.push_back(std::thread(&ThreadPool::ThreadProc, this));
		}
	}

	ThreadPool::~ThreadPool() {
		ZC_DEBUG("ThreadPool: Shutting down threads...");

		m_IsRunning = false;
		m_ThreadSignal.notify_all();

		for(std::thread& thread : m_Threads) {
			ZC_DEBUG("Waiting for Thread #" << thread.get_id() << " to shutdown...");
			thread.join();
		}		

		m_Threads.clear();
	}

	void ThreadPool::SubmitWork(const Job& job, const Callback& callback, void* context) {
		std::unique_lock<std::mutex> lock(m_JobQueueMutex);
		m_Jobs.push({job, callback, context});	
	}

	void ThreadPool::RunCallbacks() {
		std::unique_lock<std::mutex> lock(m_CallbackQueueMutex);

		for (JobData& job : m_PendingCallbacks) {
			job.callback(job.context);
		}

		m_PendingCallbacks.clear();
	}
}