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
   VkPhysicalDeviceProperties mDeviceProperties{};
   VkPhysicalDeviceFeatures mDeviceFeatures{};
   VkPhysicalDeviceMemoryProperties mDeviceMemoryProperties{};
   std::vector<VkQueueFamilyProperties> mDeviceQueueFamilyProperties;
   uint32_t mQueueFamilyCount= 0;
};

class RenderContextVulkan {

   friend class RenderContext;
   friend class std::unique_ptr<RenderContextVulkan>;
   friend struct std::unique_ptr<RenderContextVulkan>::deleter_type;

   //--------------------------------------------------------------------------
   // Ctor, creates a vk instance
   RenderContextVulkan(const RenderContextVulkanSettings &settings) :
      mSettings(settings) {
      mVkInstance = getVkInstance();
      if (!mVkInstance) {
         VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::RenderContextVulkan: Could not create vulkan instance!");
      }
      mInstanceCount++;
   }

   //--------------------------------------------------------------------------
   // D'tor
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
         std::vector<const char*> instance_extensions = { VK_KHR_SURFACE_EXTENSION_NAME,
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
         VK_CHECK_RESULT(vkCreateInstance(&instance_info, nullptr, &instance));
         return instance;
      };

      static VkInstance instance = createInstance();
      return instance;
   }


   //--------------------------------------------------------------------------
   //enumerate and select devices
   VkPhysicalDevice enumerateAndSelectDevice(const DeviceSelectionVulkan device_selection = DeviceSelectionVulkan::AUTO_SELECT) {
      // Physical device
      uint32_t gpu_count = 0;
      // Get number of available physical devices
      VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mVkInstance, &gpu_count, nullptr));
      //we dont have a suitable gpu, exit
      if (!gpu_count) {
         VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::enumerateAndSelectDevice: Could find a vulkan capable device!");
      }

      // Enumerate devices
      std::vector<VkPhysicalDevice> physical_devices(gpu_count);
      VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mVkInstance, &gpu_count, physical_devices.data()));

      //List all available gpu's
      SystemLogger::get().info("--------------------------------------------");
      SystemLogger::get().info("Found %d Vulkan capable devices", gpu_count);
      for (auto & device : physical_devices) {
         VkPhysicalDeviceProperties device_properties;
         vkGetPhysicalDeviceProperties(device, &device_properties);
         SystemLogger::get().info("--------------------------------------------");
         SystemLogger::get().info("Device Name: %s", device_properties.deviceName);
         SystemLogger::get().info("Device Id: %x", device_properties.deviceID);
         SystemLogger::get().info("Device Type: %s", VkErrorHelper::physicalDeviceTypeToStr(device_properties.deviceType).c_str());
         SystemLogger::get().info("Device API: %d.%d.%d", (device_properties.apiVersion >> 22), ((device_properties.apiVersion >> 12) & 0x3ff), (device_properties.apiVersion & 0xfff));
      }

      // GPU selection
      uint32_t selected_device_id = 0;
      VkPhysicalDevice selected_device = nullptr;

      //Manual gpu selection based on index
      if (device_selection >= DeviceSelectionVulkan::DEVICE_0) {
         selected_device_id = (unsigned)((int)device_selection - (int)DeviceSelectionVulkan::DEVICE_0);
         if (selected_device_id >= gpu_count) {
            SystemLogger::get().warn("RenderContextVulkan::enumerateAndSelectDevice: Selected device idx(%d) not available, using device idx(%d) instead!", selected_device_id, 0);
            selected_device_id = 0;
         }
         selected_device = physical_devices[selected_device_id];
      }
      //auto select most suitable gpu available
      //atm we select a discret gpu if available
      else if (device_selection == DeviceSelectionVulkan::AUTO_SELECT) {
         for (auto & device : physical_devices) {
            VkPhysicalDeviceProperties device_properties;
            VkPhysicalDeviceFeatures device_features;
            VkPhysicalDeviceMemoryProperties device_memory_properties;
            std::vector<VkQueueFamilyProperties> device_queue_family_properties;
            uint32_t queue_family_count=0;

            vkGetPhysicalDeviceProperties(device, &device_properties);
            vkGetPhysicalDeviceFeatures(device, &device_features);
            vkGetPhysicalDeviceMemoryProperties(device, &device_memory_properties);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
            device_queue_family_properties.resize(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, device_queue_family_properties.data());

            if (selected_device == nullptr) {
               mDeviceProperties.mDeviceProperties = device_properties;
               mDeviceProperties.mDeviceFeatures = device_features;
               mDeviceProperties.mDeviceMemoryProperties = device_memory_properties;
               mDeviceProperties.mDeviceQueueFamilyProperties = device_queue_family_properties;
               mDeviceProperties.mQueueFamilyCount = queue_family_count;
               selected_device = device;
            } else {
               if (  mDeviceProperties.mDeviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
                     device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                  mDeviceProperties.mDeviceProperties = device_properties;
                  mDeviceProperties.mDeviceFeatures = device_features;
                  mDeviceProperties.mDeviceMemoryProperties = device_memory_properties;
                  mDeviceProperties.mDeviceQueueFamilyProperties = device_queue_family_properties;
                  mDeviceProperties.mQueueFamilyCount = queue_family_count;
                  selected_device = device;
               }
            }
         }

         SystemLogger::get().info("Selected Adapter: %s Id:  %x", mDeviceProperties.mDeviceProperties.deviceName, mDeviceProperties.mDeviceProperties.deviceID);
         SystemLogger::get().info("Queue family count %d", mDeviceProperties.mQueueFamilyCount);
         int count = 0;
         for (auto & prop : mDeviceProperties.mDeviceQueueFamilyProperties) {
            SystemLogger::get().info("--------------------------------------------");
            SystemLogger::get().info("Queue family no. %d", count++);
            SystemLogger::get().info("Queue count: %d", prop.queueCount);
            SystemLogger::get().info("minImageTransferGranularity width: %d", prop.minImageTransferGranularity.width);
            SystemLogger::get().info("minImageTransferGranularity height: %d", prop.minImageTransferGranularity.height);
            SystemLogger::get().info("minImageTransferGranularity depth: %d", prop.minImageTransferGranularity.depth);
            SystemLogger::get().info("VK_QUEUE_GRAPHICS_BIT: %s", prop.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "YES" : "NO");
            SystemLogger::get().info("VK_QUEUE_COMPUTE_BIT: %s", prop.queueFlags & VK_QUEUE_COMPUTE_BIT ? "YES" : "NO");
            SystemLogger::get().info("VK_QUEUE_TRANSFER_BIT: %s", prop.queueFlags & VK_QUEUE_TRANSFER_BIT ? "YES" : "NO");
            SystemLogger::get().info("VK_QUEUE_SPARSE_BINDING_BIT: %s", prop.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "YES" : "NO");
         }
      }
      return mPhysicalDevice= selected_device;
   }


   DevicePropertiesVulkan mDeviceProperties{};
   RenderContextVulkanSettings mSettings{};
   VkInstance  mVkInstance = nullptr;
   VkPhysicalDevice mPhysicalDevice = nullptr;
   VkDevice    mVkDevice = nullptr;

   static std::atomic<unsigned> mInstanceCount;
};

std::atomic<unsigned> RenderContextVulkan::mInstanceCount = 0;

}
}
