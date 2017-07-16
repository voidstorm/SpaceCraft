#pragma once
#include <thread>
#include <memory>
#include <atomic>
#include <functional>

#include "CommandQueue.h"
#include "ThreadMap.h"



namespace Vt {
	//---------------------------------------------------------------------------------------
   class ThreadContext final {
	public:
		ThreadContext(const unsigned mapping, bool waitForWork = true);
		~ThreadContext();
		CommandQueue& GetCommandQueue();

      std::unique_ptr<std::function<void(void)>> OnAlwaysBegin;
      std::unique_ptr<std::function<void(void)>> OnAlwaysEnd;
	private:
		CommandQueue m_work_items;
		std::unique_ptr<std::thread> m_work_thread;
		std::atomic_bool m_running = true;
	};

	//---------------------------------------------------------------------------------------
	ThreadContext::ThreadContext(const unsigned mapping, bool waitForWork) {
		m_work_thread = std::make_unique<std::thread>([&, waitForWork]() {
         SetThreadMapping(mapping);
			while (m_running) {
				//can take a context param, e.g. the opengl or thread context.
				while (m_work_items.isWaitingForWork() && waitForWork) {
					std::this_thread::yield();
				}
            if (OnAlwaysBegin) {
               (*OnAlwaysBegin)();
            }
				m_work_items.processAllCommands(this);
            if (OnAlwaysEnd) {
               (*OnAlwaysEnd)();
            }
			}
		});
	}

	//---------------------------------------------------------------------------------------
	ThreadContext::~ThreadContext() {
		m_running.store(false);
      m_work_thread->join();
	}

	//---------------------------------------------------------------------------------------
	CommandQueue& ThreadContext::GetCommandQueue() {
		return m_work_items;
	}
}
