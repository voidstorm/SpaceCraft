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

        unsigned long long TM_WINDOW_LOOP = 0b0001;
        unsigned long long  TM_GAME_LOOP = 0b00010;
        unsigned long long  TM_RENDER_LOOP = 0b0100;

    } ThreadMapping;

    inline void SetThreadMapping(const unsigned long long mapping) { 
        //SetThreadIdealProcessor(GetCurrentThread(), mapping);
       SetThreadAffinityMask(GetCurrentThread(), mapping);
    }
}