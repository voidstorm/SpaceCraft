#pragma once
#include <thread>
#include <memory>
#include <atomic>
#include <functional>
#include <chrono>
#include <atomic>
#include <iostream>

#include "CommandQueue.h"
#include "ThreadMap.h"
#include "Delegate.h"
#include "ScopedTimer.h"



namespace Vt {
	//---------------------------------------------------------------------------------------
   class ThreadContext final {
	public:
      ThreadContext(const unsigned mapping, bool waitForWork = true) {
         m_work_thread = std::make_unique<std::thread>([&, waitForWork]() {
            SetThreadMapping(mapping);
            while (m_running.load(std::memory_order::memory_order_acquire)) {
               {
                  ScopedTimer<std::chrono::nanoseconds> sc([&, this](std::chrono::high_resolution_clock::duration &d)->void {
                     mLastDuration.store(d, std::memory_order::memory_order_release);
                  });
                  //can take a context param, e.g. the opengl or thread context.
                  while (m_work_items.isWaitingForWork() && waitForWork) {
                     std::this_thread::yield();
                  }
                  OnBeginAlways();
                  m_work_items.processAllCommands(this);
                  OnEndAlways();
               }
            }
         });
      }

      ~ThreadContext() {
         RequestExit();
      }

      void RequestExit(){
         m_running.store(false, std::memory_order::memory_order_release);
         if (m_work_thread->joinable()) {
            m_work_thread->join();
         }
      }

      CommandQueue& GetCommandQueue() {
         return m_work_items;
      }
   
      std::chrono::high_resolution_clock::duration GetDuration() const {
         return mLastDuration.load(std::memory_order::memory_order_acquire);
      }

      Delegate<void> OnBeginAlways;
      Delegate<void> OnEndAlways;

   private:
		CommandQueue m_work_items;
		std::unique_ptr<std::thread> m_work_thread;
		std::atomic_bool m_running = true;
      std::atomic<std::chrono::high_resolution_clock::duration> mLastDuration;
   };
}
