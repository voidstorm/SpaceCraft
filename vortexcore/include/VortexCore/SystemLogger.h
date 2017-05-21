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
#ifdef _DEBUG
      static auto log= std::make_unique<Logger>("vtcore.log", true, true, true);
#else
      static auto log = std::make_unique<Logger>("vtcore.log");
#endif
      return *(log.get());
   }
};
}
