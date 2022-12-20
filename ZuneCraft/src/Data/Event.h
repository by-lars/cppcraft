#pragma once
#include <vector>
#include <functional>

namespace ZuneCraft {
	template<class... Args>
	class Event {
	public:
		using Callback = std::function<void(Args...)>;

		void Subsribe(Callback c) {
			m_Listeners.push_back(c);
			ZC_DEBUG("Subscribed! size:" << m_Listeners.size() << " this=" << this);
		}

		void Invoke(Args... args) {
			ZC_DEBUG("INVOKE! size:" << m_Listeners.size() << " this="<<this);

			for (int i = 0; i < m_Listeners.size(); i++) {
				ZC_DEBUG("Calling");
				m_Listeners[i](args...);
			}
		}

	private:
		std::vector<Callback> m_Listeners;
	};
}