#pragma once
#include <memory>
#include <atomic>
#include <type_traits>
#include <vector>

#include "ForwardDecls.h"


namespace Vt {
class AlignedStorage final{
public:
   const uint32_t CACHELINE_SIZE = 64;

   AlignedStorage() {
   }

   ~AlignedStorage() {
   }

private:

};

}
