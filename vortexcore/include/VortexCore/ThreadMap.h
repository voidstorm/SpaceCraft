#pragma once
#include <Windows.h>
#include <functional>

namespace Vt {
  struct ThreadMapping_t {
    ThreadMapping_t() {
      //core i7
      if (true) {
        TM_WINDOW_LOOP = 0;
        TM_GAME_LOOP = 1;
      } else {
        //ryzen

      }
    }

    ~ThreadMapping_t() = default;

    unsigned TM_WINDOW_LOOP = 0;
    unsigned TM_GAME_LOOP = 1;

  } ThreadMapping;

  inline void SetThreadMapping(const unsigned mapping) {
    SetThreadIdealProcessor(GetCurrentThread(), mapping);
  }
}