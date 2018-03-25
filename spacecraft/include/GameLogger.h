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

#ifdef GAME_LOGGING_ON
#define GAME_LOG_INFO(x, ...) Sc::GameLogger::get().info(x, ##__VA_ARGS__)
#define GAME_LOG_WARN(x, ...) Sc::GameLogger::get().warn(x, ##__VA_ARGS__)
#define GAME_LOG_ERROR(x, ...) Sc::GameLogger::get().error(x, ##__VA_ARGS__)
#else 
#define GAME_LOG_INFO(x, ...)
#define GAME_LOG_WARN(x, ...)
#define GAME_LOG_ERROR(x, ...)
#endif
