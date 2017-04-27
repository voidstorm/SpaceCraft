#include "VortexCore\Game.h"
#include "VortexCore\SceneManager.h"
#include "VortexCore\RenderContext.h"
#include "VortexCore\AppWindow.h"

#include <chrono>
#include <thread>
#include <iostream>

//if we dont have an ini file, create a default window
Vt::Game::Game(std::unique_ptr<Vt::Gfx::RenderContext> &&render_context):
    mRenderContext(std::move(render_context)) {
    //attach callbacks
    auto window = mRenderContext->window().lock();
    if ( window ) {
        window->OnCreate += [this] ()->void {
            this->mRunning = true;
        };

        window->OnClose += [this] ()->bool {
            return this->requestShutdown();
        };
        //resize window to fit render context
        window->setSize(mRenderContext->layout().width, mRenderContext->layout().height);
    } else {
        throw std::exception("Vt::Game::Game: failed to get a valid window!");
    }
    //Create scene manager
    mSceneManager = std::make_unique<Vt::Scene::SceneManager>(this->renderContext());
}


Vt::Game::~Game() {
    requestShutdown();
}

Vt::Gfx::RenderContext& Vt::Game::renderContext() {
    return *mRenderContext.get();
}

bool Vt::Game::requestShutdown() {
    mRunning = false;

    if ( !mRunning ) {
        while ( !mShutdown ) {
            std::this_thread::yield();
        }
    }
    return !mRunning;
}

Vt::Scene::SceneManager & Vt::Game::sceneManager() {
    return *mSceneManager.get();
}

int Vt::Game::shutdown() {
    //shutdown render context
    this->mRenderContext.reset();
    return 0;
}

void Vt::Game::loop(const std::chrono::high_resolution_clock::duration & delta) {

}

std::future<int> Vt::Game::start() {
    return std::async(std::launch::async,[this] ()->int {
        while ( !mRunning ) {
            //wait for window to show
        }
        //gameloop
        auto now = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::high_resolution_clock::now() - now;
        while ( mRunning ) {
            mLastTick = now;
            loop(dur = ( now = std::chrono::high_resolution_clock::now() ) - mLastTick);
#ifdef VT_TIMING
            if ( mCounter < 100 ) {
                mTiming += (double)std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000>>>( dur ).count();
                mCounter++;
            } else {
                std::cout << "[VT_TIMING]  Vt::Game::loop: " << ( mTiming / double(mCounter) ) << " milliseconds" << std::endl;
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
