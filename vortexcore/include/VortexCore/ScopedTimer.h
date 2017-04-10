#pragma once
#include <chrono>
#include <functional>

namespace Vt {
	//------------------------------------------------------------------------------------------------
	//Executes a callback when going out of scope that returns elapsed time.
	template < typename T = std::chrono::milliseconds>
	class ScopedTimer {
	public:
		typedef std::chrono::duration<double, std::ratio<1, 1>> fseconds;

		ScopedTimer(std::function<void(T)> on_out_of_scope) :
			m_callback(on_out_of_scope) {
		}

		~ScopedTimer() {
			m_callback(std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - m_start));
		}

	private:
		std::function<void(T d)> m_callback;
		std::chrono::high_resolution_clock::time_point m_start = std::chrono::high_resolution_clock::now();
	};
}
