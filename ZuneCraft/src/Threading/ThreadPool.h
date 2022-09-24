#pragma once
#include "Core/Service.h"
#include "Threading/Mutex.h"
#include "Threading/Signal.h"
#include <Windows.h>

namespace ZuneCraft {
	class ThreadPool : public IService {
	public:

		struct Job {
			Job() {
				JobFunction = nullptr;
				CallbackFunction = nullptr;
				Context = nullptr;
			}

			void (*JobFunction)(void* context);
			void (*CallbackFunction)(void* context);
			void* Context;
		};

		ThreadPool();
		~ThreadPool();

		void Init() override;
		void SubmitWork(const Job& job);
		void RunCallbacks();

		static void ThreadProc(LPVOID instance);

	private:
		std::vector<Job> m_Jobs;
		std::vector<Job> m_PendingCallbacks;
		std::vector<HANDLE> m_Threads;

		Mutex m_JobMutex;
		Signal m_JobSignal;

		bool m_IsRunning;
	};
}