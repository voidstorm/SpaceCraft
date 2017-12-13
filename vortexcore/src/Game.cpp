#include "VortexCore\Game.h"
#include "VortexCore\SceneManager.h"
#include "VortexCore\RenderContext.h"
#include "VortexCore/AppWindow.h"
#include "VortexCore/ThreadMap.h"
#include "VortexCore/ThreadContext.h"
#include "VortexCore/ScopedWaitTimer.h"
#include "VortexCore/SystemLogger.h"


#include <chrono>
#include <thread>
#include <iostream>

#include <windows.h>

//if we dont have an ini file, create a default window
Vt::Game::Game(std::unique_ptr<Vt::Gfx::RenderContext> &&render_context) :
   mRenderContext(std::move(render_context)) {
   //attach callbacks
   auto window = mRenderContext->window().lock();
   if (window) {
      //resize window to fit render context
      window->setSize(mRenderContext->layout().width, mRenderContext->layout().height);

      window->OnCreate += [this]()->void {
          SYSTEM_LOG_INFO("-------------------------------------------------------");
          SYSTEM_LOG_INFO("Schnarzmaschine, version %s", SCHNARZ_VERSION);
          SYSTEM_LOG_INFO("-------------------------------------------------------");
          SYSTEM_LOG_INFO("Booting...");
          //Init render context
         try {
            mRenderContext->init();
         } catch (const std::exception& e) {
            //fail
            VT_EXCEPT_RT(GameException, "Game:::Game Could not init render context!", e);
         }
         //Create scene manager
         mSceneManager = std::make_unique<Vt::Scene::SceneManager>(this->renderContext());
         this->mRunning.store(true, std::memory_order::memory_order_release);
      };

      window->OnClose += [this]()->bool {
         return this->requestShutdown();
      };
   } else {
      throw std::exception("Vt::Game::Game: failed to get a valid window!");
   }
}


//-----------------------------------------------------------------
//
Vt::Game::~Game() {
   requestShutdown();
}

//-----------------------------------------------------------------
//
Vt::Gfx::RenderContext& Vt::Game::renderContext() {
   return *mRenderContext.get();
}

//-----------------------------------------------------------------
// can be overwritten, if returns false, window will not close
bool Vt::Game::requestShutdown() {
   if (mRunning) {
      mRunning = false;
      if (!mRunning) {
         auto shutdown_result = shutdown();
      }
   }
   return !mRunning;
}

//-----------------------------------------------------------------
Vt::Scene::SceneManager & Vt::Game::sceneManager() {
   return *mSceneManager.get();
}

//-----------------------------------------------------------------
//
int Vt::Game::shutdown() {
   mGameThread->RequestExit();
   mGameThread.reset();
   mRenderThread->RequestExit();
   mRenderThread.reset();
   //unload scenes
   mSceneManager.reset();
   //shutdown render context
   mRenderContext.reset();
   return 0;
}

//-----------------------------------------------------------------
//
void Vt::Game::tick(const std::chrono::high_resolution_clock::duration & delta) {
   auto activeScenes = mSceneManager->activeScenes();
   for (auto & scene : activeScenes) {
      scene->tick(delta);
   }
}

//-----------------------------------------------------------------
//
void Vt::Game::draw(const std::chrono::high_resolution_clock::duration & delta) {
   auto visibleScenes = mSceneManager->visibleScenes();
   for (auto & scene : visibleScenes) {
      scene->draw(delta);
   }
}

//-----------------------------------------------------------------
//
std::future<int> Vt::Game::start() {
   return std::async(std::launch::async, [this]()->int {
      while (!mRunning.load(std::memory_order::memory_order_acquire)) {
         //wait for window to show
         std::this_thread::yield();
      }
      //load init resources
      mSceneManager->loadResources();

      //render thread
      mRenderThread = std::make_unique<Vt::ThreadContext>(Vt::ThreadMapping.TM_RENDER_LOOP, false);
      mRenderThread->OnBeginAlways += [&, this](void)->void {
         draw(mRenderThread->GetDuration());

#ifdef VT_TIMING
         if (mTiming < 1000) {
            mTiming += (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(mRenderThread->GetDuration()).count();
            mCounter++;
         } else {
            auto ms = (mTiming / double(mCounter));
            std::cout << "[VT_TIMING]  RenderThread::tick: " << ms << " milliseconds, " << 1000.0 / ms << "fps" << std::endl;
            mCounter = 0;
            mTiming = 0.0;
         }
#endif
      };
      mRenderThread->OnEndAlways += [&, this](void)->void {
         mRenderContext->swapBuffers();
      };

      //game thread
      mGameThread = std::make_unique<Vt::ThreadContext>(Vt::ThreadMapping.TM_GAME_LOOP, false);
      mGameThread->OnBeginAlways += [&, this](void)->void {
         tick(mGameThread->GetDuration());
         //#ifdef VT_TIMING
         //         if (mTiming < 1000) {
         //            mTiming += (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(mGameThread->GetDuration()).count();
         //            mCounter++;
         //         } else {
         //            auto ms = (mTiming / double(mCounter));
         //            std::cout << "[VT_TIMING]  GameThread::tick: " << ms << " milliseconds, " << 1000.0 / ms << "fps" << std::endl;
         //            mCounter = 0;
         //            mTiming = 0.0;
         //         }
         //#endif
      };

      return 0;
   });
}

//-----------------------------------------------------------------
//
Vt::ThreadContext & Vt::Game::renderThread() {
   return *mRenderThread;
}

//-----------------------------------------------------------------
//
Vt::ThreadContext & Vt::Game::gameThread() {
   return *mGameThread;
}
