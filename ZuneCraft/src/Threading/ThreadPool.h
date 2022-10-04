#pragma once
#include "Core/Service.h"
#include "Threading/Mutex.h"
#include "Threading/Signal.h"
#include <Windows.h>

namespace ZuneCraft {
	class ThreadPool : public IService {
	public:
		typedef void (*function_t)(void* context);

		struct Job {
			Job() {
				JobFunction = nullptr;
				CallbackFunction = nullptr;
				Context = nullptr;
			}

			function_t JobFunction;
			function_t CallbackFunction;
			void* Context;
		};

		ThreadPool();
		~ThreadPool();

		void SubmitWork(function_t jobFunction, function_t callbackFunction, void* context);
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