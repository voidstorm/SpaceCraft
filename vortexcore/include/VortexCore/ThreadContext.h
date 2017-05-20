#pragma once
#include <thread>
#include <memory>
#include <atomic>

#include "CommandQueue.h"
#include "ThreadMap.h"



namespace Vt {
	//---------------------------------------------------------------------------------------
   class ThreadContext final {
	public:
		ThreadContext(const unsigned mapping);
		~ThreadContext();
		CommandQueue& GetCommandQueue();
	private:
		CommandQueue m_work_items;
		std::unique_ptr<std::thread> m_work_thread;
		std::atomic_bool m_running = true;
	};

	//---------------------------------------------------------------------------------------
	ThreadContext::ThreadContext(const unsigned mapping) {
		m_work_thread = std::make_unique<std::thread>([&]() {
         SetThreadMapping(mapping);
			while (m_running) {
				//can take a context param, e.g. the opengl or thread context.
				while (m_work_items.isWaitingForWork()) {
					std::this_thread::yield();
				}
				m_work_items.processAllCommands(this);
			}
		});
	}

	//---------------------------------------------------------------------------------------
	ThreadContext::~ThreadContext() {
		m_running.store(false);
	}

	//---------------------------------------------------------------------------------------
	CommandQueue& ThreadContext::GetCommandQueue() {
		return m_work_items;
	}
}
