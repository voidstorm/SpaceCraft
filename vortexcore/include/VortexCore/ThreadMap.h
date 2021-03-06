#pragma once
#include <Windows.h>
#include <functional>
#include <thread>

namespace Vt {
struct ThreadMapping_t {
   ThreadMapping_t() {
      //core i7
      if (true) {
         //TM_WINDOW_LOOP = 0;
         //TM_GAME_LOOP = 1;
         //TM_RENDER_LOOP = 2;
      } else {
         //ryzen

      }
   }

   ~ThreadMapping_t() = default;

   unsigned long long TM_WINDOW_LOOP = 0;
   unsigned long long  TM_GAME_LOOP = 1;
   unsigned long long  TM_RENDER_LOOP = 2;

};

extern ThreadMapping_t ThreadMapping;

inline void SetThreadMapping(const unsigned long long mapping) {
   SetThreadIdealProcessor(GetCurrentThread(), (DWORD)mapping);
   //SetThreadAffinityMask(GetCurrentThread(), mapping);
}
}