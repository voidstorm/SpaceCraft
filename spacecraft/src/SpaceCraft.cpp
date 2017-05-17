// SpaceCraft.cpp : Defines the entry point for the console application.
//
#include "VortexCore\AppWindow.h"
#include "VortexCore\Game.h"
#include "VortexCore\RenderContext.h"

#include <iostream>
#include <exception>


int main(int argc, char** argv) {
  VT_DO_LEAK_CHECK;
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
    //run window loop
    return Vt::App::AppWindow::exec();
  } catch ( const std::exception& e ) {
    std::cout << e.what() << std::endl;
    return -1;
  }
  return 0;
}

