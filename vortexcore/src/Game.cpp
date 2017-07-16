#include "VortexCore\Game.h"
#include "VortexCore\SceneManager.h"
#include "VortexCore\RenderContext.h"
#include "VortexCore\AppWindow.h"
#include "VortexCore\ThreadMap.h"

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
      window->OnCreate += [this]()->void {
         this->mRunning = true;
      };

      window->OnClose += [this]()->bool {
         return this->requestShutdown();
      };
      //resize window to fit render context
      window->setSize(mRenderContext->layout().width, mRenderContext->layout().height);
   } else {
      throw std::exception("Vt::Game::Game: failed to get a valid window!");
   }
   //Init render context
   try {
      mRenderContext->init();
   } catch (const std::exception& e) {
      //fail
      VT_EXCEPT_RT(GameException, "Game:::Game Could not init render context!", e);
   }
   //Create scene manager
   mSceneManager = std::make_unique<Vt::Scene::SceneManager>(this->renderContext());
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
   mRunning = false;

   if (!mRunning) {
      while (!mShutdown) {
         std::this_thread::yield();
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
   //unload scenes
   mSceneManager = nullptr;
   //shutdown render context
   mRenderContext= nullptr;
   return 0;
}

//-----------------------------------------------------------------
//
void Vt::Game::loop(const std::chrono::high_resolution_clock::duration & delta) {
   mRenderContext->swapBuffers();
}

//-----------------------------------------------------------------
//
std::future<int> Vt::Game::start() {
   return std::async(std::launch::async, [this]()->int {
      Vt::SetThreadMapping(Vt::ThreadMapping.TM_GAME_LOOP);
      while (!mRunning) {
         //wait for window to show
          std::this_thread::yield();
      }
      //init render context


      auto now = std::chrono::high_resolution_clock::now();
      auto dur = std::chrono::high_resolution_clock::now() - now;
      //start other concurrent loops...

      //gameloop
      while (mRunning) {
         mLastTick = now;
         loop(dur = ((now = std::chrono::high_resolution_clock::now()) - mLastTick));
#ifdef VT_TIMING
         if (mTiming < 1000) {
            mTiming += (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(dur).count();
            mCounter++;
         } else {
            auto ms = (mTiming / double(mCounter));
            std::cout << "[VT_TIMING]  Vt::Game::loop: " << ms << " milliseconds, " << 1000.0 / ms << "fps" << std::endl;
            mCounter = 0;
            mTiming = 0.0;
         }
#endif
      }
      auto shutdown_result = shutdown();
      mShutdown = true;
      return shutdown_result;
   });
}
