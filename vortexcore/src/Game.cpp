#include "VortexCore\Game.h"
#include "VortexCore\SceneManager.h"
#include "VortexCore\RenderContext.h"
#include "VortexCore\AppWindow.h"
#include "VortexCore\ThreadMap.h"
#include "VortexCore/ThreadContext.h"

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

         //Init render context
         try {
            mRenderContext->init();
         } catch (const std::exception& e) {
            //fail
            VT_EXCEPT_RT(GameException, "Game:::Game Could not init render context!", e);
         }
         //Create scene manager
         mSceneManager = std::make_unique<Vt::Scene::SceneManager>(this->renderContext());

         this->mRunning = true;
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
   mRunning = false;

   if (!mRunning) {
      auto shutdown_result = shutdown();
      mShutdown = true;
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
   mGameThread = nullptr;
   mRenderThread = nullptr;
   //unload scenes
   mSceneManager = nullptr;
   //shutdown render context
   mRenderContext = nullptr;
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
      while (!mRunning) {
         //wait for window to show
         std::this_thread::yield();
      }
      //load init resources
      mSceneManager->loadResources();
      {
         //render thread
         auto now = std::chrono::high_resolution_clock::now();
         auto dur = std::chrono::high_resolution_clock::now() - now;
         auto last_tick = std::chrono::high_resolution_clock::now();
         double timing = 0;
         int counter = 0;
         mRenderThread = std::make_unique<Vt::ThreadContext>(Vt::ThreadMapping.TM_RENDER_LOOP, false);
         mRenderThread->OnAlwaysBegin = std::make_unique<std::function<void(void)>>([&, this](void)->void {
            last_tick = now;
            draw(dur = ((now = std::chrono::high_resolution_clock::now()) - last_tick));
#ifdef VT_TIMING
            if (timing < 1000) {
               timing += (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(dur).count();
               counter++;
            } else {
               auto ms = (timing / double(counter));
               std::cout << "[VT_TIMING]  Vt::Game::draw: " << ms << " milliseconds, " << 1000.0 / ms << "fps" << std::endl;
               counter = 0;
               timing = 0.0;
            }
#endif
         });
      }

      //game thread
      {
         mGameThread = std::make_unique<Vt::ThreadContext>(Vt::ThreadMapping.TM_GAME_LOOP, false);
         auto now = std::chrono::high_resolution_clock::now();
         auto dur = std::chrono::high_resolution_clock::now() - now;
         mGameThread->OnAlwaysBegin = std::make_unique<std::function<void(void)>>([&, this](void)->void {
            mLastTick = now;
            tick(dur = ((now = std::chrono::high_resolution_clock::now()) - mLastTick));
#ifdef VT_TIMING
            if (mTiming < 1000) {
               mTiming += (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(dur).count();
               mCounter++;
            } else {
               auto ms = (mTiming / double(mCounter));
               std::cout << "[VT_TIMING]  Vt::Game::tick: " << ms << " milliseconds, " << 1000.0 / ms << "fps" << std::endl;
               mCounter = 0;
               mTiming = 0.0;
            }
#endif
         });
      }
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
