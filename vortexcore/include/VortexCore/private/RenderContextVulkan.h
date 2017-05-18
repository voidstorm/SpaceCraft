#pragma once
#include <memory>
#include <atomic>
#include "vulkan\vulkan.h"


namespace Vt {
  namespace Gfx {
    class RenderContextVulkan {

      friend class RenderContext;
      friend class std::unique_ptr<RenderContextVulkan>;
      friend struct std::unique_ptr<RenderContextVulkan>::deleter_type;

      RenderContextVulkan(){
        mVkInstance = getVkInstance();
        mInstanceCount++;
      }

      ~RenderContextVulkan() {
        mInstanceCount--;
        if (mInstanceCount == 0) {
          //release vulkan instance and device
        }
      }

      VkInstance getVkInstance() {
        auto create_instance = [this]()->VkInstance {

        };
        static VkInstance vk_instance = create_instance();
        return vk_instance;
      }



      VkInstance  mVkInstance= nullptr;
      VkDevice    mVkDevice= nullptr;

      static std::atomic<unsigned> mInstanceCount;
    };

    std::atomic<unsigned> RenderContextVulkan::mInstanceCount= 0;

  }
}
