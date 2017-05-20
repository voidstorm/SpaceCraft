#pragma once
#include "VortexCore\Logger.h"


namespace Sc {


class GameLogger {
public:

   GameLogger() {

   }
   
   ~GameLogger() {

   }

   static Vt::Logger & get() {
      static auto log= std::make_unique<Vt::Logger>("game.log");
      return *(log.get());
   }
};
}
