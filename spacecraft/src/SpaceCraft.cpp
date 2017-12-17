// SpaceCraft.cpp : Defines the entry point for the console application.
//
#include "VortexCore\AppWindow.h"
#include "VortexCore\Game.h"
#include "VortexCore\SceneManager.h"
#include "VortexCore\RenderContext.h"
#include "VortexCore\ThreadMap.h"

#include "GameLogger.h"
#include "SceneTest.h"

#include <iostream>
#include <exception>
#include <thread>
#include <future>

namespace Vt {
   ThreadMapping_t ThreadMapping;
}

void loadTestScenes(Vt::Game& game);

int main(int argc, char** argv) {
   VT_DO_LEAK_CHECK;
   Vt::SetThreadMapping(Vt::ThreadMapping.TM_WINDOW_LOOP);
   //default framebuffer
   Vt::Gfx::RenderContextLayout rcl = {
       1280,   //width
       720,    //height
       0,      //screen
       false,  //fullscreen
       0,      //depthbuffer
       0,      //stencil
       0       //multisample
   };

   try {
      //Create window
      auto app = Vt::App::AppWindow::create(L"SpaceCraft");

      //create game object
      auto game = std::make_unique<Vt::Game>(std::make_unique<Vt::Gfx::RenderContext>(app.lock(), rcl));
      //run game loop
      auto game_result = game->start();

      //add test scenes
      auto r= std::async(std::launch::async,[&]()->void {
         game_result.get();
         loadTestScenes(*game);
      });

      //run window loop
      return Vt::App::AppWindow::exec();
   } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
      Sc::GameLogger::get().error(e.what());
      return -1;
   }
   return 0;
}

void loadTestScenes(Vt::Game& game) {
   auto& sceneManager = game.sceneManager();
   //scene test
   auto &testscene= sceneManager.addScene(std::make_unique<Sc::SceneTest>(sceneManager));
   //load
   testscene.load();
   //activate
   testscene.activate();
   testscene.show();
 
}
