#pragma once
#include <chrono>
#include <functional>

namespace Vt {
	//Waits the remaining time before going out of scope and then 
	//calls the callback and returns elapsed time.
	//Default wait mode is yield. For durations < 2ms one should use wait mode spin.
	struct WaitTypeSpin {
	};
	struct WaitTypeYield {
	};

	template < typename T = std::chrono::milliseconds, typename WT = WaitTypeYield>
	class ScopedWaitTimer {
	private:
		std::chrono::high_resolution_clock::time_point m_start = std::chrono::high_resolution_clock::now();
		T m_timeout;
		std::function<void(T d)> m_callback;
	public:
		typedef std::chrono::duration<double, std::ratio<1, 1>> fseconds;

		ScopedWaitTimer(T timeout, std::function<void(T)> on_out_of_scope) :
			m_timeout(timeout),
			m_callback(on_out_of_scope) {
		}

		~ScopedWaitTimer() {
			Wait();
			m_callback(std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - m_start));
		}

	private:
		template <typename U = WT>
		inline typename std::enable_if<std::is_same<U, WaitTypeSpin>::value, void>::type
			Wait() {
			while ((std::chrono::high_resolution_clock::now() - m_start) < m_timeout) {
				//spin
			}
		}

		template <typename U = WT>
		inline typename std::enable_if<std::is_same<U, WaitTypeYield>::value, void>::type
			Wait() {
			while ((std::chrono::high_resolution_clock::now() - m_start) < m_timeout) {
				std::this_thread::yield();
			}
		}
	};
}