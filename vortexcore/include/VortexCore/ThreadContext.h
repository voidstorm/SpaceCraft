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



namespace Vt {
	//---------------------------------------------------------------------------------------
   class ThreadContext final {
	public:
      ThreadContext(const unsigned mapping, bool waitForWork = true) {
         m_work_thread = std::make_unique<std::thread>([&, waitForWork]() {
            SetThreadMapping(mapping);
            auto lastTime = std::chrono::high_resolution_clock::now();
            while (m_running) {
               auto now = std::chrono::high_resolution_clock::now();
               mLastDuration = now - lastTime;
               lastTime = now;
               //can take a context param, e.g. the opengl or thread context.
               while (m_work_items.isWaitingForWork() && waitForWork) {
                  std::this_thread::yield();
               }
               OnBeginAlways();
               m_work_items.processAllCommands(this);
               OnEndAlways();
            }
         });
      }

      ~ThreadContext() {
         RequestExit();
      }

      void RequestExit(){
         m_running.store(false);
         if (m_work_thread->joinable()) {
            m_work_thread->join();
         }
      }

      CommandQueue& GetCommandQueue() {
         return m_work_items;
      }
   
      std::chrono::high_resolution_clock::duration GetDuration() const {
         return mLastDuration;
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
