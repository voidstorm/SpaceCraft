#pragma once
#include "Logger.h"


namespace Vt {


class SystemLogger {
public:

   SystemLogger() {

   }
   
   ~SystemLogger() {

   }

   static Logger & get() {
      static auto log= std::make_unique<Logger>("vtcore.log");
      return *(log.get());
   }

private:
   std::unique_ptr<Logger> mLog;
};
}
