#pragma once
#include "Core/Service.h"

#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace ZuneCraft {
	class ThreadPool : public IService {
	public:
		ThreadPool();
		~ThreadPool();

		using Job = std::function<void(void*)>;
		using Callback = std::function<void(void*)>;

		struct JobData {
			Job job;
			Callback callback;
			void* context;
		};

		void SubmitWork(const Job& job, const Callback& callback, void* context);
		void RunCallbacks();


	private:
		std::condition_variable m_ThreadSignal;
		std::mutex m_JobQueueMutex;
		std::queue<JobData> m_Jobs;

		std::mutex m_CallbackQueueMutex;
		std::vector<JobData> m_PendingCallbacks;

		std::vector<std::thread> m_Threads;
		std::atomic<bool> m_IsRunning;
		
		void ThreadProc();
	};
}