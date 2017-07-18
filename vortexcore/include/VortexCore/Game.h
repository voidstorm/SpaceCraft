#pragma once
#include "Api.h"
#include <memory>
#include <thread>
#include <future>
#include <atomic>
#include <exception>
#include "ForwardDecls.h"

namespace Vt {

class GameException : public std::runtime_error {
public:
   explicit GameException(const std::string& what_arg) :std::runtime_error(what_arg) {};
   explicit GameException(const char* what_arg) :std::runtime_error(what_arg) {};
};


//The main game class housing the game loop and holding everything together. 
//Needs to be attached to an application window.


//Holds several thread contexts than can be processed in parallel. E.g there are two scenegraphs doing
//double buffering so that rendering and input/update/physics can be done in parallell. Also AI can be done this
//way...

class VORTEX_API Game {
public:
   Game(std::unique_ptr<Vt::Gfx::RenderContext> &&render_context);
   virtual ~Game();

   virtual Vt::Gfx::RenderContext& renderContext();
   virtual bool requestShutdown();
   virtual Vt::Scene::SceneManager & sceneManager();
   virtual std::future<int> start();
   
   Vt::ThreadContext& renderThread();
   Vt::ThreadContext& gameThread();

protected:
   //shuts everything down
   virtual int shutdown();
   //runs the main game loop
   virtual void tick(const std::chrono::high_resolution_clock::duration & delta);
   //runs the render loop
   virtual void draw(const std::chrono::high_resolution_clock::duration & delta);

protected:
   std::atomic_bool mRunning = false;
   std::atomic_bool mShutdown = false;
   std::unique_ptr<Gfx::RenderContext> mRenderContext;
   std::unique_ptr<Scene::SceneManager> mSceneManager;
   std::unique_ptr<Vt::ThreadContext> mRenderThread;
   std::unique_ptr<Vt::ThreadContext> mGameThread;


#ifdef VT_TIMING
   double mTiming = 0.0;
   int    mCounter = 0;
#endif
};
}

