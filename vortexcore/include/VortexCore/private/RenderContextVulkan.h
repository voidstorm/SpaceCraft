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
         VkPhysicalDeviceProperties device_properties;
         VkPhysicalDeviceFeatures device_features;
         VkPhysicalDeviceMemoryProperties device_memory_properties;
         std::vector<VkQueueFamilyProperties> device_queue_family_properties;
         uint32_t queue_family_count = 0;

         vkGetPhysicalDeviceProperties(selected_device, &device_properties);
         vkGetPhysicalDeviceFeatures(selected_device, &device_features);
         vkGetPhysicalDeviceMemoryProperties(selected_device, &device_memory_properties);
         vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &queue_family_count, nullptr);
         device_queue_family_properties.resize(queue_family_count);
         vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &queue_family_count, device_queue_family_properties.data());

         mDeviceProperties.mDeviceProperties = device_properties;
         mDeviceProperties.mDeviceFeatures = device_features;
         mDeviceProperties.mDeviceMemoryProperties = device_memory_properties;
         mDeviceProperties.mDeviceQueueFamilyProperties = device_queue_family_properties;
         mDeviceProperties.mQueueFamilyCount = queue_family_count;

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

            logAdapterProperties(device_properties, device_queue_family_properties, device_features, device_memory_properties);

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
      }
      SystemLogger::get().info("Selected Adapter: %s Id:  %x", mDeviceProperties.mDeviceProperties.deviceName, mDeviceProperties.mDeviceProperties.deviceID);
      return mPhysicalDevice= selected_device;
   }

   //-----------------------------------------------------------------
   //log some useful adapter info
   void logAdapterProperties( const VkPhysicalDeviceProperties & device_properties, 
                              const std::vector<VkQueueFamilyProperties> & queue_properties,
                              const VkPhysicalDeviceFeatures & features,
                              const VkPhysicalDeviceMemoryProperties & memory_info) {

      SystemLogger::get().info("--------------------------------------------");
      SystemLogger::get().info("Detailed Device Properties");
      SystemLogger::get().info("--------------------------------------------");
      SystemLogger::get().info("Adapter: %s", device_properties.deviceName);
      SystemLogger::get().info("Queue family count %d", queue_properties.size());
      int count = 0;
      for (auto & prop : queue_properties) {
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
      SystemLogger::get().info("--------------------------------------------");
      SystemLogger::get().info("MEMORY PROPERTIES");
      SystemLogger::get().info("memoryTypeCount: %d", memory_info.memoryTypeCount);
      SystemLogger::get().info("Memory types:");
      for (int i = 0; i < memory_info.memoryTypeCount; ++i) {
         SystemLogger::get().info("--------------------------------------------");
         SystemLogger::get().info("heapIndex: %d", memory_info.memoryTypes[i].heapIndex);
         SystemLogger::get().info("VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT: %s", memory_info.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? "YES" : "NO");
         SystemLogger::get().info("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: %s", memory_info.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ? "YES" : "NO");
         SystemLogger::get().info("VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: %s", memory_info.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ? "YES" : "NO");
         SystemLogger::get().info("VK_MEMORY_PROPERTY_HOST_CACHED_BIT: %s", memory_info.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT ? "YES" : "NO");
         SystemLogger::get().info("VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT: %s", memory_info.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT ? "YES" : "NO");
      }
      SystemLogger::get().info("--------------------------------------------");
      SystemLogger::get().info("memoryHeapCount: %d", memory_info.memoryHeapCount);
      SystemLogger::get().info("Memory Heaps:");
      for (int i = 0; i < memory_info.memoryHeapCount; ++i) {
         SystemLogger::get().info("size: %u MB", (unsigned)(memory_info.memoryHeaps[i].size / (1024LL* 1024LL)));
         SystemLogger::get().info("VK_MEMORY_HEAP_DEVICE_LOCAL_BIT: %s", memory_info.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "YES" : "NO");
         SystemLogger::get().info("VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX: %s", memory_info.memoryHeaps[i].flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX ? "YES" : "NO");
      }

      SystemLogger::get().info("--------------------------------------------");
      SystemLogger::get().info("FEATURE SET");
      SystemLogger::get().info("robustBufferAccess: %s", features.robustBufferAccess ? "YES" : "NO");
      SystemLogger::get().info("fullDrawIndexUint32: %s",features.fullDrawIndexUint32 ? "YES" : "NO");
      SystemLogger::get().info("imageCubeArray: %s",features.imageCubeArray ? "YES" : "NO");
      SystemLogger::get().info("independentBlend: %s",features.independentBlend ? "YES" : "NO");
      SystemLogger::get().info("geometryShader: %s",features.geometryShader ? "YES" : "NO");
      SystemLogger::get().info("tessellationShader: %s",features.tessellationShader ? "YES" : "NO");
      SystemLogger::get().info("sampleRateShading: %s",features.sampleRateShading ? "YES" : "NO");
      SystemLogger::get().info("dualSrcBlend: %s",features.dualSrcBlend ? "YES" : "NO");
      SystemLogger::get().info("logicOp: %s",features.logicOp ? "YES" : "NO");
      SystemLogger::get().info("multiDrawIndirect: %s",features.multiDrawIndirect ? "YES" : "NO");
      SystemLogger::get().info("drawIndirectFirstInstance: %s",features.drawIndirectFirstInstance ? "YES" : "NO");
      SystemLogger::get().info("depthClamp: %s",features.depthClamp ? "YES" : "NO");
      SystemLogger::get().info("depthBiasClamp: %s",features.depthBiasClamp ? "YES" : "NO");
      SystemLogger::get().info("fillModeNonSolid: %s",features.fillModeNonSolid ? "YES" : "NO");
      SystemLogger::get().info("depthBounds: %s",features.depthBounds ? "YES" : "NO");
      SystemLogger::get().info("wideLines: %s",features.wideLines ? "YES" : "NO");
      SystemLogger::get().info("largePoints: %s",features.largePoints ? "YES" : "NO");
      SystemLogger::get().info("alphaToOne: %s",features.alphaToOne ? "YES" : "NO");
      SystemLogger::get().info("multiViewport: %s",features.multiViewport ? "YES" : "NO");
      SystemLogger::get().info("samplerAnisotropy: %s",features.samplerAnisotropy ? "YES" : "NO");
      SystemLogger::get().info("textureCompressionETC2: %s",features.textureCompressionETC2 ? "YES" : "NO");
      SystemLogger::get().info("textureCompressionASTC_LDR: %s",features.textureCompressionASTC_LDR ? "YES" : "NO");
      SystemLogger::get().info("textureCompressionBC: %s",features.textureCompressionBC ? "YES" : "NO");
      SystemLogger::get().info("occlusionQueryPrecise: %s",features.occlusionQueryPrecise ? "YES" : "NO");
      SystemLogger::get().info("pipelineStatisticsQuery: %s",features.pipelineStatisticsQuery ? "YES" : "NO");
      SystemLogger::get().info("vertexPipelineStoresAndAtomics: %s",features.vertexPipelineStoresAndAtomics ? "YES" : "NO");
      SystemLogger::get().info("fragmentStoresAndAtomics: %s",features.fragmentStoresAndAtomics ? "YES" : "NO");
      SystemLogger::get().info("shaderTessellationAndGeometryPointSize: %s",features.shaderTessellationAndGeometryPointSize ? "YES" : "NO");
      SystemLogger::get().info("shaderImageGatherExtended: %s",features.shaderImageGatherExtended ? "YES" : "NO");
      SystemLogger::get().info("shaderStorageImageExtendedFormats: %s",features.shaderStorageImageExtendedFormats ? "YES" : "NO");
      SystemLogger::get().info("shaderStorageImageMultisample: %s",features.shaderStorageImageMultisample ? "YES" : "NO");
      SystemLogger::get().info("shaderStorageImageReadWithoutFormat: %s",features.shaderStorageImageReadWithoutFormat ? "YES" : "NO");
      SystemLogger::get().info("shaderStorageImageWriteWithoutFormat: %s",features.shaderStorageImageWriteWithoutFormat ? "YES" : "NO");
      SystemLogger::get().info("shaderUniformBufferArrayDynamicIndexing: %s",features.shaderUniformBufferArrayDynamicIndexing ? "YES" : "NO");
      SystemLogger::get().info("shaderSampledImageArrayDynamicIndexing: %s",features.shaderSampledImageArrayDynamicIndexing ? "YES" : "NO");
      SystemLogger::get().info("shaderStorageBufferArrayDynamicIndexing: %s",features.shaderStorageBufferArrayDynamicIndexing ? "YES" : "NO");
      SystemLogger::get().info("shaderStorageImageArrayDynamicIndexing: %s",features.shaderStorageImageArrayDynamicIndexing ? "YES" : "NO");
      SystemLogger::get().info("shaderClipDistance: %s",features.shaderClipDistance ? "YES" : "NO");
      SystemLogger::get().info("shaderCullDistance: %s",features.shaderCullDistance ? "YES" : "NO");
      SystemLogger::get().info("shaderFloat64: %s",features.shaderFloat64 ? "YES" : "NO");
      SystemLogger::get().info("shaderInt64: %s",features.shaderInt64 ? "YES" : "NO");
      SystemLogger::get().info("shaderInt16: %s",features.shaderInt16 ? "YES" : "NO");
      SystemLogger::get().info("shaderResourceResidency: %s",features.shaderResourceResidency ? "YES" : "NO");
      SystemLogger::get().info("shaderResourceMinLod: %s",features.shaderResourceMinLod ? "YES" : "NO");
      SystemLogger::get().info("sparseBinding: %s",features.sparseBinding ? "YES" : "NO");
      SystemLogger::get().info("sparseResidencyBuffer: %s",features.sparseResidencyBuffer ? "YES" : "NO");
      SystemLogger::get().info("sparseResidencyImage2D: %s",features.sparseResidencyImage2D ? "YES" : "NO");
      SystemLogger::get().info("sparseResidencyImage3D: %s",features.sparseResidencyImage3D ? "YES" : "NO");
      SystemLogger::get().info("sparseResidency2Samples: %s",features.sparseResidency2Samples ? "YES" : "NO");
      SystemLogger::get().info("sparseResidency4Samples: %s",features.sparseResidency4Samples ? "YES" : "NO");
      SystemLogger::get().info("sparseResidency8Samples: %s",features.sparseResidency8Samples ? "YES" : "NO");
      SystemLogger::get().info("sparseResidency16Samples: %s",features.sparseResidency16Samples ? "YES" : "NO");
      SystemLogger::get().info("sparseResidencyAliased: %s",features.sparseResidencyAliased ? "YES" : "NO");
      SystemLogger::get().info("variableMultisampleRate: %s",features.variableMultisampleRate ? "YES" : "NO");
      SystemLogger::get().info("inheritedQueries: %s",features.inheritedQueries ? "YES" : "NO");
      SystemLogger::get().info("--------------------------------------------");
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
