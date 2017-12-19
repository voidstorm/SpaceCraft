#pragma once
#include "Interface.h"
#include <atomic>

namespace Vt {
class QueryProgress : public Interface {
public:
   QueryProgress() {
   
   };
   virtual ~QueryProgress() {
   
   };

   float progress() const {
      return mProgress.load(std::memory_order::memory_order_acquire);
   }

   void set(const float value) {
      mProgress.store(value, std::memory_order::memory_order_release);
   }

   void reset() {
      mProgress.store(0.0f, std::memory_order::memory_order_release);
   }

private:
   std::atomic<float> mProgress = 0.0f;
};
}
