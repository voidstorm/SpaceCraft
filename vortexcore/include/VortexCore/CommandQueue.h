#pragma once
#include <memory>
#include <functional>
#include <queue>
#include <future>
#include <atomic>
#include <condition_variable>
#include <boost\variant\variant.hpp>

namespace Vt {
	//---------------------------------------------------------------------------------------
	class CommandQueue {
	public:
		typedef boost::variant<bool, int, float, double, void*, std::string> CMD_RET_TYPE;
		//typedef boost::any MSG_RET_TYPE;
		typedef std::function<CMD_RET_TYPE(void*)> CMD_FUN_TYPE;

		//---------------------------------------------------------------------------------------
		CommandQueue() {
			m_waiting_for_work.test_and_set(std::memory_order_acquire);
			m_commands_push = &m_commands0;
			m_commands_pop = &m_commands1;
		}

		//---------------------------------------------------------------------------------------
		~CommandQueue() {
		}

		//---------------------------------------------------------------------------------------
		std::future<CommandQueue::CMD_RET_TYPE> Submit(CommandQueue::CMD_FUN_TYPE &&msg) {
			auto p = std::promise<CMD_RET_TYPE>();
			auto f = p.get_future();
			while (m_busy.test_and_set(std::memory_order_acquire));
			m_commands_push->emplace(msg, std::move(p));
			m_waiting_for_work.clear(std::memory_order_release);
			m_busy.clear(std::memory_order_release);
			return f;
		}


		//---------------------------------------------------------------------------------------
		size_t GetSize() {
			return m_commands_push->size();
		}

	private:
		//---------------------------------------------------------------------------------------
		void swap() {
			while (m_busy.test_and_set(std::memory_order_acquire));
			std::swap(m_commands_push, m_commands_pop);
			m_busy.clear(std::memory_order_release);
		}

		//---------------------------------------------------------------------------------------
		void processAllCommands(void* context) {
			swap();
			while (!m_commands_pop->empty()) {
				auto &m = m_commands_pop->front();
				(std::get<1>(m)).set_value(std::get<0>(m)(context));
				m_commands_pop->pop();
			}
		}

		//---------------------------------------------------------------------------------------
		bool isWaitingForWork() {
			return m_waiting_for_work.test_and_set(std::memory_order_acquire);
		}


		std::queue<std::tuple<CMD_FUN_TYPE, std::promise<CMD_RET_TYPE>>> m_commands0;
		std::queue<std::tuple<CMD_FUN_TYPE, std::promise<CMD_RET_TYPE>>> m_commands1;
		std::queue<std::tuple<CMD_FUN_TYPE, std::promise<CMD_RET_TYPE>>> *m_commands_push;
		std::queue<std::tuple<CMD_FUN_TYPE, std::promise<CMD_RET_TYPE>>> *m_commands_pop;
		std::atomic_flag m_busy = ATOMIC_FLAG_INIT;
		std::atomic_flag m_waiting_for_work = ATOMIC_FLAG_INIT;
		bool m_false = false;
		friend class ThreadContext;
	};
}

