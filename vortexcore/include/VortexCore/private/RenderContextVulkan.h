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

class SwapchainVulkan;

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

struct SwapchainSettingsVulkan {
   VkSurfaceFormatKHR mSurfaceFormat{ VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
   VkPresentModeKHR mPresentMode{ VK_PRESENT_MODE_IMMEDIATE_KHR }; //no v-sync
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
   bool mPresentQueueExclusive = false;
};

struct QueueFamilyIndex {
   uint32_t mGraphics = 0;
   uint32_t mCompute = 0;
   uint32_t mTransfer = 0;
   uint32_t mPresent = 0;
};

enum class QueueType {
   GRAPHICS= 0,
   COMPUTE,
   TRANSFER,
   PRESENT
};

class RenderContextVulkan {

   friend class RenderContext;
   friend class SwapchainVulkan;
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
   VkInstance vkInstance();

   //--------------------------------------------------------------------------
   //retrieve physical vulkan device
   VkPhysicalDevice vkPhysicalDevice();

   //--------------------------------------------------------------------------
   //retrieve logical vulkan device
   VkDevice vkDevice();

   //--------------------------------------------------------------------------
   //enumerate and select devices
   VkPhysicalDevice enumerateAndSelectDevice(const DeviceSelectionVulkan device_selection = DeviceSelectionVulkan::AUTO_SELECT);

   //-----------------------------------------------------------------
   // Finds the proper queue family index
   uint32_t findQueueFamilyIndex(const VkQueueFlagBits queueFlags);

   //-----------------------------------------------------------------
   // Finds the proper present queue family index
   uint32_t findPresentQueueFamilyIndex();

   //-----------------------------------------------------------------
   // Creates a logical device
   VkDevice createDevice(const VkPhysicalDevice device, const QueueCreationInfo & queueCreateInfo);

   //-----------------------------------------------------------------
   // Creates a swapchain
   std::weak_ptr<Vt::Gfx::SwapchainVulkan> createWindowSurfaceAndSwapchain(const Vt::App::AppWindow & window, const SwapchainSettingsVulkan & swapchainSettings);

   //-----------------------------------------------------------------
   //log some useful adapter info
   void logAdapterProperties( const VkPhysicalDeviceProperties & deviceProperties, 
                              const std::vector<VkQueueFamilyProperties> & queueProperties,
                              const VkPhysicalDeviceFeatures & features,
                              const VkPhysicalDeviceMemoryProperties & memoryInfo,
      const std::vector<std::string> & extensions);


   //-----------------------------------------------------------------
   // Returns the proper queue family index of the created device
   uint32_t queueFamilyIndex(QueueType type) const;

   //-----------------------------------------------------------------
   // Returns the queue count
   uint32_t queueCount(QueueType type) const;

  
   //-----------------------------------------------------------------
   // Returns a specific device queue
   VkQueue deviceQueue(QueueType type, uint32_t index) const;

   //-----------------------------------------------------------------
   // Returns if a device extension is supported
   bool checkInstanceExtension(const std::string &extension);

   //-----------------------------------------------------------------
   // Returns if a device extension is supported
   bool checkDeviceExtension(const std::string &extension);

   //-----------------------------------------------------------------
   // Returns if a device feature is supported
   VkPhysicalDeviceFeatures deviceFeatures() const;

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
   std::shared_ptr<SwapchainVulkan> mSwapchain;
   VkInstance                       mVkInstance{ nullptr };
   VkPhysicalDevice                 mPhysicalDevice{ nullptr };
   VkDevice                         mVkDevice{ nullptr };

   DevicePropertiesVulkan           mDeviceProperties{};
   RenderContextVulkanSettings      mContextSettings{};
   QueueFamilyIndex                 mQueueIndices{};
   QueueCreationInfo                mQueueConfiguration{};

   static VkDebugReportCallbackEXT sMsgCallback;
   static std::atomic<unsigned>     mInstanceCount;
};

}
}
