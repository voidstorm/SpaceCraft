#pragma once
#include <memory>
#include <atomic>
#include <vector>
#include <exception>
#include "vulkan\vulkan.h"

#include "..\SystemLogger.h"
#include "VkErrorHelper.h"

namespace Vt {
namespace Gfx {

class RenderContextVkException : public std::runtime_error {
public:
   explicit RenderContextVkException(const std::string& what_arg) :std::runtime_error(what_arg) {};
   explicit RenderContextVkException(const char* what_arg) :std::runtime_error(what_arg) {};
};

struct RenderContextVulkanSettings {
   enum class ValidationFlags : unsigned {
      NONE = 0,
      STANDARD = 1,
      ALL = 2
   };
   ValidationFlags mValidation = ValidationFlags::NONE;
};

enum class DeviceSelectionVulkan : unsigned {
   AUTO_SELECT,
   CPU,
   GPU,
   IGPU,
   DEVICE_0,
   DEVICE_1,
   DEVICE_2,
   DEVICE_3,
};

struct DevicePropertiesVulkan {
   uint32_t mQueueFamilyCount = 0;
   VkPhysicalDeviceProperties mDeviceProperties{};
   VkPhysicalDeviceFeatures mDeviceFeatures{};
   VkPhysicalDeviceMemoryProperties mDeviceMemoryProperties{};
   std::vector<VkQueueFamilyProperties> mDeviceQueueFamilyProperties;
   std::vector<std::string> mExtensions;
   bool mDebugMarkers = false;
};

struct QueueCreationInfo {
   enum class QueueCount {
      _1= 1,
      _2,
      _3,
      _4,
      _5,
      _7,
      MAX = 16
   };
   QueueCount mGfxQueueCount = QueueCount::_1;
   QueueCount mComputeQueueCount = QueueCount::_1;
   QueueCount mTransferQueueCount = QueueCount::_1;
   bool mGfxQueueExclusive = false;
   bool mComputeQueueExclusive = true;
   bool mTransferQueueExclusive = true;
};

struct QueueFamilyIndex {
   uint32_t mGraphics = 0;
   uint32_t mCompute = 0;
   uint32_t mTransfer = 0;
};

class RenderContextVulkan {

   friend class RenderContext;
   friend class std::unique_ptr<RenderContextVulkan>;
   friend struct std::unique_ptr<RenderContextVulkan>::deleter_type;

   //--------------------------------------------------------------------------
   // Ctor, creates a vk instance
   RenderContextVulkan(const RenderContextVulkanSettings &settings);

   //--------------------------------------------------------------------------
   // D'tor
   ~RenderContextVulkan();

   //--------------------------------------------------------------------------
   //retrieve vulkan instance
   VkInstance getVkInstance();


   //--------------------------------------------------------------------------
   //enumerate and select devices
   VkPhysicalDevice enumerateAndSelectDevice(const DeviceSelectionVulkan device_selection = DeviceSelectionVulkan::AUTO_SELECT);

   //-----------------------------------------------------------------
   // Finds the proper queue family index
   uint32_t getQueueFamilyIndex(const VkQueueFlagBits queueFlags);


   //-----------------------------------------------------------------
   // Creates a logical device
   VkDevice createDevice(const VkPhysicalDevice device, const QueueCreationInfo & queueCreateInfo);

   //-----------------------------------------------------------------
   //log some useful adapter info
   void logAdapterProperties( const VkPhysicalDeviceProperties & deviceProperties, 
                              const std::vector<VkQueueFamilyProperties> & queueProperties,
                              const VkPhysicalDeviceFeatures & features,
                              const VkPhysicalDeviceMemoryProperties & memoryInfo,
      const std::vector<std::string> & extensions);

   //--------------------------------------------------------------------------
   //members
   static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(
      VkDebugReportFlagsEXT flags,
      VkDebugReportObjectTypeEXT objType,
      uint64_t obj,
      size_t location,
      int32_t code,
      const char* layerPrefix,
      const char* msg);


   //--------------------------------------------------------------------------
   //members
   static VKAPI_ATTR VkBool32 VKAPI_CALL sValidationLayerCallback(
      VkDebugReportFlagsEXT flags,
      VkDebugReportObjectTypeEXT objType,
      uint64_t obj,
      size_t location,
      int32_t code,
      const char* layerPrefix,
      const char* msg,
      void* userData);

   static PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
   static PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
   static PFN_vkDebugReportMessageEXT dbgBreakCallback;


   //--------------------------------------------------------------------------
   //members
   VkInstance                       mVkInstance = nullptr;
   VkPhysicalDevice                 mPhysicalDevice = nullptr;
   VkDevice                         mVkDevice = nullptr;

   DevicePropertiesVulkan           mDeviceProperties{};
   RenderContextVulkanSettings      mContextSettings{};
   QueueFamilyIndex                 mQueueIndices{};
   QueueCreationInfo                mQueueConfiguration{};

   static VkDebugReportCallbackEXT sMsgCallback;
   static std::atomic<unsigned>     mInstanceCount;
};

}
}
