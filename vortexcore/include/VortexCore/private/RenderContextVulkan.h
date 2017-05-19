#pragma once
#include <memory>
#include <atomic>
#include <vector>
#include <exception>
#include "vulkan\vulkan.h"

#include "..\Logger.h"


namespace Vt {
namespace Gfx {

class RenderContextVkException : public std::runtime_error {
public:
   explicit RenderContextVkException(const std::string& what_arg) :std::runtime_error(what_arg) {};
   explicit RenderContextVkException(const char* what_arg) :std::runtime_error(what_arg) {};
};


struct RenderContextVulkanSettings {
   enum class ValidationFlags : unsigned {
      NONE= 0,
      STANDARD=1,
      ALL= 2
   };
   ValidationFlags mValidation= ValidationFlags::NONE;
};

class RenderContextVulkan {

   friend class RenderContext;
   friend class std::unique_ptr<RenderContextVulkan>;
   friend struct std::unique_ptr<RenderContextVulkan>::deleter_type;

   //--------------------------------------------------------------------------
   //
   RenderContextVulkan(const RenderContextVulkanSettings &settings):
      mSettings(settings){
      mVkInstance = getVkInstance();
      if (!mVkInstance) {
         VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::RenderContextVulkan: Could not create vulkan instance!");
      }
      mInstanceCount++;
   }

   //--------------------------------------------------------------------------
   //
   ~RenderContextVulkan() {
      mInstanceCount--;
      if (mInstanceCount == 0) {
         //release vulkan instance and device
         if (mVkInstance) {
            vkDestroyInstance(mVkInstance, nullptr);
            mVkInstance = nullptr;
         }
      }
   }

   //--------------------------------------------------------------------------
   //retrieve vulkan instance
   VkInstance getVkInstance() {
      auto createInstance = [this]()->VkInstance {

         //application info
         VkApplicationInfo app_info = {};
         app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
         app_info.pApplicationName = "VortexCore";
         app_info.pEngineName = "VortexCore";
         app_info.apiVersion = VK_API_VERSION_1_0;
         
         //instance extension
         std::vector<const char*> instance_extensions = {  VK_KHR_SURFACE_EXTENSION_NAME,
                                                           VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

         //instance info
         VkInstanceCreateInfo instance_info = {};
         instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
         instance_info.pNext = NULL;
         instance_info.pApplicationInfo = &app_info;
         
         //debugging and validation support
         if (instance_extensions.size() > 0) {
            if (mSettings.mValidation >= RenderContextVulkanSettings::ValidationFlags::STANDARD) {
               instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            }

            instance_info.enabledExtensionCount = (uint32_t)instance_extensions.size();
            instance_info.ppEnabledExtensionNames = instance_extensions.data();
         }

         if (mSettings.mValidation == RenderContextVulkanSettings::ValidationFlags::STANDARD) {
            int32_t  validationLayerCount = 2;
            const char *validationLayerNames[] = {
               "VK_LAYER_LUNARG_standard_validation",
               "VK_LAYER_LUNARG_monitor"
            };

            instance_info.enabledLayerCount = validationLayerCount;
            instance_info.ppEnabledLayerNames = validationLayerNames;
         } else if (mSettings.mValidation == RenderContextVulkanSettings::ValidationFlags::ALL) {
            int32_t validationLayerCount = 2;
            const char *validationLayerNames[] = {
               "VK_LAYER_LUNARG_standard_validation",
               "VK_LAYER_LUNARG_monitor"
            };

            instance_info.enabledLayerCount = validationLayerCount;
            instance_info.ppEnabledLayerNames = validationLayerNames;
         }

         VkInstance instance = nullptr;
         auto result = vkCreateInstance(&instance_info, nullptr, &instance);
         if (result != VkResult::VK_SUCCESS) {
            return nullptr;
         }

         Logger::getInstance().log(LogLevel::LOG_INFO, "Vulkan instance created with result %d", (int)result);

         return instance;
      };

      static VkInstance instance = createInstance();
      return instance;
   }


   RenderContextVulkanSettings mSettings;
   VkInstance  mVkInstance = nullptr;
   VkDevice    mVkDevice = nullptr;

   static std::atomic<unsigned> mInstanceCount;
};

std::atomic<unsigned> RenderContextVulkan::mInstanceCount = 0;

}
}
