#pragma once
#include <Windows.h>

namespace Vt {
  enum class ThreadMapping : unsigned {
     TM_WINDOW_LOOP= 0
    ,TM_GAME_LOOP= 1
  };

  inline void SetThreadMapping(ThreadMapping m) {
    SetThreadIdealProcessor(GetCurrentThread(), static_cast<unsigned>(m));
  }
}