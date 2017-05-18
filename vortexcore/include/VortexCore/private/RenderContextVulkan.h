#pragma once
#include <memory>
#include "vulkan\vulkan.h"


namespace Vt {
  namespace Gfx {
    class RenderContextVulkan {

      RenderContextVulkan() {

      }

      ~RenderContextVulkan() {

      }

      friend class RenderContext;
      friend class std::unique_ptr<RenderContextVulkan>;
      friend struct std::unique_ptr<RenderContextVulkan>::deleter_type;
    };
  }
}
