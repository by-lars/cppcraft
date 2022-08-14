#pragma once

#define ZC_PROFILE_FUNCTION ProfilingTimer

namespace ZuneCraft {

	class ProfilingTimer {
	public:
		ProfilingTimer(const char* name);
		~ProfilingTimer();

	private:
		const char* m_Name;
	};
}