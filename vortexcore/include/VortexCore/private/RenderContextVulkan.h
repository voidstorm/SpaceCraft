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
      MAX = 100
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
         if (mVkDevice) {
            vkDestroyDevice(mVkDevice, nullptr);
            mVkDevice = nullptr;
            SYSTEM_LOG_INFO("Vulkan Device released!");
         }
         if (mVkInstance) {
            vkDestroyInstance(mVkInstance, nullptr);
            mVkInstance = nullptr;
            SYSTEM_LOG_INFO("Vulkan Instance released!");
         }
      }
   }

   //--------------------------------------------------------------------------
   //retrieve vulkan instance
   VkInstance getVkInstance() {
      auto createInstance = [this]()->VkInstance {

         //application info
         VkApplicationInfo appInfo = {};
         appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
         appInfo.pApplicationName = "VortexCore";
         appInfo.pEngineName = "VortexCore";
         appInfo.apiVersion = VK_API_VERSION_1_0;

         //instance extension
         std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME,
                                                           VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

         //instance info
         VkInstanceCreateInfo instanceInfo = {};
         instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
         instanceInfo.pNext = NULL;
         instanceInfo.pApplicationInfo = &appInfo;

         //debugging and validation support
         if (instanceExtensions.size() > 0) {
            if (mSettings.mValidation >= RenderContextVulkanSettings::ValidationFlags::STANDARD) {
               instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            }

            instanceInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
            instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
         }

         if (mSettings.mValidation == RenderContextVulkanSettings::ValidationFlags::STANDARD) {
            int32_t  validationLayerCount = 2;
            const char *validationLayerNames[] = {
               "VK_LAYER_LUNARG_standard_validation",
               "VK_LAYER_LUNARG_monitor"
            };

            instanceInfo.enabledLayerCount = validationLayerCount;
            instanceInfo.ppEnabledLayerNames = validationLayerNames;
         } else if (mSettings.mValidation == RenderContextVulkanSettings::ValidationFlags::ALL) {
            int32_t validationLayerCount = 2;
            const char *validationLayerNames[] = {
               "VK_LAYER_LUNARG_standard_validation",
               "VK_LAYER_LUNARG_monitor"
            };

            instanceInfo.enabledLayerCount = validationLayerCount;
            instanceInfo.ppEnabledLayerNames = validationLayerNames;
         }

         VkInstance instance = nullptr;
         VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &instance));

         SYSTEM_LOG_INFO("Created Instance with enabled layers:");
         for (int i = 0; i < (int)instanceInfo.enabledLayerCount; ++i){
             SYSTEM_LOG_INFO("%s", instanceInfo.ppEnabledLayerNames[i]);
         }

         return instance;
      };

      static VkInstance instance = createInstance();
      return instance;
   }


   //--------------------------------------------------------------------------
   //enumerate and select devices
   VkPhysicalDevice enumerateAndSelectDevice(const DeviceSelectionVulkan device_selection = DeviceSelectionVulkan::AUTO_SELECT) {
      // Physical device
      uint32_t gpuCount = 0;
      // Get number of available physical devices
      VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mVkInstance, &gpuCount, nullptr));
      //we dont have a suitable gpu, exit
      if (!gpuCount) {
         VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::enumerateAndSelectDevice: Could find a vulkan capable device!");
      }

      // Enumerate devices
      std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
      VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mVkInstance, &gpuCount, physicalDevices.data()));

      //List all available gpu's
      SYSTEM_LOG_INFO("--------------------------------------------");
      SYSTEM_LOG_INFO("Found %d Vulkan capable devices", gpuCount);
      for (auto & device : physicalDevices) {
         VkPhysicalDeviceProperties deviceProperties;
         vkGetPhysicalDeviceProperties(device, &deviceProperties);
         SYSTEM_LOG_INFO("--------------------------------------------");
         SYSTEM_LOG_INFO("Device Name: %s", deviceProperties.deviceName);
         SYSTEM_LOG_INFO("Device Id: %x", deviceProperties.deviceID);
         SYSTEM_LOG_INFO("Device Type: %s", VkErrorHelper::physicalDeviceTypeToStr(deviceProperties.deviceType).c_str());
         SYSTEM_LOG_INFO("Device API: %d.%d.%d", (deviceProperties.apiVersion >> 22), ((deviceProperties.apiVersion >> 12) & 0x3ff), (deviceProperties.apiVersion & 0xfff));
      }

      // GPU selection
      uint32_t selectedDeviceId = 0;
      VkPhysicalDevice selectedDevice = nullptr;

      //Manual gpu selection based on index
      if (device_selection >= DeviceSelectionVulkan::DEVICE_0) {
         selectedDeviceId = (unsigned)((int)device_selection - (int)DeviceSelectionVulkan::DEVICE_0);
         if (selectedDeviceId >= gpuCount) {
            SYSTEM_LOG_WARN("RenderContextVulkan::enumerateAndSelectDevice: Selected device idx(%d) not available, using device idx(%d) instead!", selectedDeviceId, 0);
            selectedDeviceId = 0;
         }
         selectedDevice = physicalDevices[selectedDeviceId];
         VkPhysicalDeviceProperties deviceProperties;
         VkPhysicalDeviceFeatures deviceFeatures;
         VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
         std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties;
         uint32_t queueFamilyCount = 0;

         vkGetPhysicalDeviceProperties(selectedDevice, &deviceProperties);
         vkGetPhysicalDeviceFeatures(selectedDevice, &deviceFeatures);
         vkGetPhysicalDeviceMemoryProperties(selectedDevice, &deviceMemoryProperties);
         vkGetPhysicalDeviceQueueFamilyProperties(selectedDevice, &queueFamilyCount, nullptr);
         deviceQueueFamilyProperties.resize(queueFamilyCount);
         vkGetPhysicalDeviceQueueFamilyProperties(selectedDevice, &queueFamilyCount, deviceQueueFamilyProperties.data());

         mDeviceProperties.mDeviceProperties = deviceProperties;
         mDeviceProperties.mDeviceFeatures = deviceFeatures;
         mDeviceProperties.mDeviceMemoryProperties = deviceMemoryProperties;
         mDeviceProperties.mDeviceQueueFamilyProperties = deviceQueueFamilyProperties;
         mDeviceProperties.mQueueFamilyCount = queueFamilyCount;
      }
      //auto select most suitable gpu available
      //atm we select a discret gpu if available
      else if (device_selection == DeviceSelectionVulkan::AUTO_SELECT) {
         for (auto & device : physicalDevices) {
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
            std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties;
            uint32_t queueFamilyCount=0;

            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
            vkGetPhysicalDeviceMemoryProperties(device, &deviceMemoryProperties);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
            deviceQueueFamilyProperties.resize(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, deviceQueueFamilyProperties.data());

            //enumerate and store extensions supported by selected device
            uint32_t extCount = 0;
            std::vector<std::string> extensionNames;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
            if (extCount > 0) {
               std::vector<VkExtensionProperties> extensions(extCount);
               if (vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, extensions.data()) == VK_SUCCESS) {
                  for (auto ext : extensions) {
                     extensionNames.push_back(ext.extensionName);
                  }
               }
            }

            logAdapterProperties(deviceProperties, deviceQueueFamilyProperties, deviceFeatures, deviceMemoryProperties, extensionNames);

            if (selectedDevice == nullptr) {
               mDeviceProperties.mDeviceProperties = deviceProperties;
               mDeviceProperties.mDeviceFeatures = deviceFeatures;
               mDeviceProperties.mDeviceMemoryProperties = deviceMemoryProperties;
               mDeviceProperties.mDeviceQueueFamilyProperties = deviceQueueFamilyProperties;
               mDeviceProperties.mQueueFamilyCount = queueFamilyCount;
               mDeviceProperties.mExtensions = extensionNames;
               selectedDevice = device;
            } else {
               if (  mDeviceProperties.mDeviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
                     deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                  mDeviceProperties.mDeviceProperties = deviceProperties;
                  mDeviceProperties.mDeviceFeatures = deviceFeatures;
                  mDeviceProperties.mDeviceMemoryProperties = deviceMemoryProperties;
                  mDeviceProperties.mDeviceQueueFamilyProperties = deviceQueueFamilyProperties;
                  mDeviceProperties.mQueueFamilyCount = queueFamilyCount;
                  mDeviceProperties.mExtensions = extensionNames;
                  selectedDevice = device;
               }
            }
         }
      }
      SYSTEM_LOG_INFO("Selected Adapter: %s Id:  %x", mDeviceProperties.mDeviceProperties.deviceName, mDeviceProperties.mDeviceProperties.deviceID);
      if (!mDeviceProperties.mQueueFamilyCount) {
         VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::enumerateAndSelectDevice: Could find a vulkan capable device!");
      }
      return mPhysicalDevice= selectedDevice;
   }

   //-----------------------------------------------------------------
   // Finds the proper queue family index
   uint32_t getQueueFamilyIndex(const VkQueueFlagBits queueFlags) {
      // Dedicated queue for compute
      // Try to find a queue family index that supports compute but not graphics
      if (queueFlags & VK_QUEUE_COMPUTE_BIT) {
         for (uint32_t i = 0; i < static_cast<uint32_t>(mDeviceProperties.mDeviceQueueFamilyProperties.size()); i++) {
            if ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & queueFlags) && ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
               return i;
               break;
            }
         }
      }

      // Dedicated queue for transfer
      // Try to find a queue family index that supports transfer but not graphics and compute
      if (queueFlags & VK_QUEUE_TRANSFER_BIT) {
         for (uint32_t i = 0; i < static_cast<uint32_t>(mDeviceProperties.mDeviceQueueFamilyProperties.size()); i++) {
            if ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & queueFlags) && 
                 ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && 
                  ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
               return i;
               break;
            }
         }
      }

      // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
      for (uint32_t i = 0; i < static_cast<uint32_t>(mDeviceProperties.mDeviceQueueFamilyProperties.size()); i++) {
         if (mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & queueFlags) {
            return i;
            break;
         }
      }
      SYSTEM_LOG_ERROR("RenderContextVulkan::getQueueFamilyIndex: Could not find a matching queue index!");
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::getQueueFamilyIndex: Could not find a matching queue index!");
   }


   //-----------------------------------------------------------------
   // Creates a logical device
   VkDevice createDevice(const VkPhysicalDevice device, const QueueCreationInfo & queueCreateInfo) {

      mQueueConfiguration = queueCreateInfo;
      std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

      // Get queue family indices for the requested queue family types
      // Note that the indices may overlap depending on the implementation

      const float defaultQueuePriority(0.0f);

      // Graphics queue
      if ((int)queueCreateInfo.mGfxQueueCount > 0 ) {
         mQueueIndices.mGraphics = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
         VkDeviceQueueCreateInfo queueInfo{};
         queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
         queueInfo.queueFamilyIndex = mQueueIndices.mGraphics;
         queueInfo.queueCount = queueCreateInfo.mGfxQueueCount != QueueCreationInfo::QueueCount::MAX ? (int)queueCreateInfo.mGfxQueueCount : 
                                                                  mDeviceProperties.mDeviceQueueFamilyProperties[mQueueIndices.mGraphics].queueCount;
         queueInfo.pQueuePriorities = &defaultQueuePriority;
         queueCreateInfos.push_back(queueInfo);
         mQueueConfiguration.mGfxQueueCount = (QueueCreationInfo::QueueCount)queueInfo.queueCount;
      } else {
         mQueueIndices.mGraphics = VK_NULL_HANDLE;
         SYSTEM_LOG_ERROR("RenderContextVulkan::createDevice: Could not find a matching queue index for GfxQueue!");
         VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::createDevice: Could not find a matching queue index for GfxQueue!");
      }

      // Dedicated compute queue
      if ((int)queueCreateInfo.mComputeQueueCount > 0) {
         mQueueIndices.mCompute = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
         if (mQueueIndices.mCompute != mQueueIndices.mGraphics && queueCreateInfo.mComputeQueueExclusive) {
            // If compute family index differs, we need an additional queue create info for the compute queue
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = mQueueIndices.mCompute;
            queueInfo.queueCount = queueCreateInfo.mComputeQueueCount != QueueCreationInfo::QueueCount::MAX ? (int)queueCreateInfo.mComputeQueueCount :
                                                                     mDeviceProperties.mDeviceQueueFamilyProperties[mQueueIndices.mCompute].queueCount;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
            mQueueConfiguration.mComputeQueueCount = (QueueCreationInfo::QueueCount)queueInfo.queueCount;
            mQueueConfiguration.mComputeQueueExclusive = true;
         }
      } else if(mQueueIndices.mCompute == mQueueIndices.mGraphics && !queueCreateInfo.mComputeQueueExclusive){
         // Else we use the same queue
         mQueueIndices.mCompute = mQueueIndices.mGraphics;
         mQueueConfiguration.mComputeQueueExclusive = false;
      }
      else {
         mQueueIndices.mCompute = mQueueIndices.mGraphics;
         mQueueConfiguration.mComputeQueueExclusive = false;
         SYSTEM_LOG_WARN("RenderContextVulkan::createDevice: Exclusive compute queue requested, but not available, using gfx queue instead!");
      }

      // Dedicated transfer queue
      if ((int)queueCreateInfo.mTransferQueueCount > 0) {
         mQueueIndices.mTransfer = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
         if (mQueueIndices.mTransfer != mQueueIndices.mGraphics && queueCreateInfo.mTransferQueueExclusive) {
            // If compute family index differs, we need an additional queue create info for the compute queue
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = mQueueIndices.mTransfer;
            queueInfo.queueCount = queueCreateInfo.mTransferQueueCount != QueueCreationInfo::QueueCount::MAX ? (int)queueCreateInfo.mTransferQueueCount :
               mDeviceProperties.mDeviceQueueFamilyProperties[mQueueIndices.mTransfer].queueCount;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
            mQueueConfiguration.mTransferQueueCount = (QueueCreationInfo::QueueCount)queueInfo.queueCount;
            mQueueConfiguration.mTransferQueueExclusive = true;
         }
      } else if (mQueueIndices.mTransfer == mQueueIndices.mGraphics && !queueCreateInfo.mTransferQueueExclusive) {
         // Else we use the same queue
         mQueueIndices.mTransfer = mQueueIndices.mGraphics;
         mQueueConfiguration.mTransferQueueExclusive = false;
      } else {
         mQueueIndices.mTransfer = mQueueIndices.mGraphics;
         mQueueConfiguration.mTransferQueueExclusive = false;
         SYSTEM_LOG_WARN("RenderContextVulkan::createDevice: Exclusive transfer queue requested, but not available, using gfx queue instead!");
      }

      //push all extensions
      std::vector<const char*> deviceExtensions;
      for (auto & ext : mDeviceProperties.mExtensions) {
         // Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
         if (ext == std::string(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
            mDeviceProperties.mDebugMarkers = true;
         }
         deviceExtensions.push_back(ext.c_str());
      }

      VkDeviceCreateInfo deviceCreateInfo = {};
      deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
      deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
      
      //deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
      //we enable all features the device supports
      deviceCreateInfo.pEnabledFeatures = &mDeviceProperties.mDeviceFeatures;

      if (deviceExtensions.size() > 0) {
         deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
         deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
      }

      // Create the logical device representation
      VkDevice logicalDevice;
      VK_CHECK_RESULT(vkCreateDevice(device, &deviceCreateInfo, nullptr, &logicalDevice));

      SYSTEM_LOG_INFO("-----------------------------------------------------------------------");
      SYSTEM_LOG_INFO("Created device with queue configuration:");
      SYSTEM_LOG_INFO("Graphics Queue Idx: %d", mQueueIndices.mGraphics);
      SYSTEM_LOG_INFO("Graphics Queue Count: %d", mQueueConfiguration.mGfxQueueCount);
      SYSTEM_LOG_INFO("Graphics Queue Exclusive: %s", mQueueConfiguration.mGfxQueueExclusive ? "YES" : "NO");

      SYSTEM_LOG_INFO("Compute Queue Idx: %d", mQueueIndices.mCompute);
      SYSTEM_LOG_INFO("Compute Queue Count: %d", mQueueConfiguration.mComputeQueueCount);
      SYSTEM_LOG_INFO("Compute Queue Exclusive: %s", mQueueConfiguration.mComputeQueueExclusive ? "YES" : "NO");

      SYSTEM_LOG_INFO("Transfer Queue Idx: %d", mQueueIndices.mTransfer);
      SYSTEM_LOG_INFO("Transfer Queue Count: %d", mQueueConfiguration.mTransferQueueCount);
      SYSTEM_LOG_INFO("Transfer Queue Exclusive: %s", mQueueConfiguration.mTransferQueueExclusive ? "YES" : "NO");

      //// Create a default command pool for graphics command buffers
      //commandPool = createCommandPool(queueFamilyIndices.graphics); 
      return mVkDevice = logicalDevice;
   }

   //-----------------------------------------------------------------
   //log some useful adapter info
   void logAdapterProperties( const VkPhysicalDeviceProperties & deviceProperties, 
                              const std::vector<VkQueueFamilyProperties> & queueProperties,
                              const VkPhysicalDeviceFeatures & features,
                              const VkPhysicalDeviceMemoryProperties & memoryInfo,
                              const std::vector<std::string> & extensions) {

      SYSTEM_LOG_INFO("--------------------------------------------");
      SYSTEM_LOG_INFO("Detailed Device Properties");
      SYSTEM_LOG_INFO("--------------------------------------------");
      SYSTEM_LOG_INFO("Adapter: %s", deviceProperties.deviceName);
      SYSTEM_LOG_INFO("Queue family count %d", queueProperties.size());
      int count = 0;
      for (auto & prop : queueProperties) {
         SYSTEM_LOG_INFO("--------------------------------------------");
         SYSTEM_LOG_INFO("Queue family idx. %d", count++);
         SYSTEM_LOG_INFO("Queue count: %d", prop.queueCount);
         SYSTEM_LOG_INFO("minImageTransferGranularity width: %d", prop.minImageTransferGranularity.width);
         SYSTEM_LOG_INFO("minImageTransferGranularity height: %d", prop.minImageTransferGranularity.height);
         SYSTEM_LOG_INFO("minImageTransferGranularity depth: %d", prop.minImageTransferGranularity.depth);
         SYSTEM_LOG_INFO("VK_QUEUE_GRAPHICS_BIT: %s", prop.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_QUEUE_COMPUTE_BIT: %s", prop.queueFlags & VK_QUEUE_COMPUTE_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_QUEUE_TRANSFER_BIT: %s", prop.queueFlags & VK_QUEUE_TRANSFER_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_QUEUE_SPARSE_BINDING_BIT: %s", prop.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "YES" : "NO");
      }
      SYSTEM_LOG_INFO("--------------------------------------------");
      SYSTEM_LOG_INFO("MEMORY PROPERTIES");
      SYSTEM_LOG_INFO("memoryTypeCount: %d", memoryInfo.memoryTypeCount);
      SYSTEM_LOG_INFO("Memory types:");
      for (unsigned i = 0; i < memoryInfo.memoryTypeCount; ++i) {
         SYSTEM_LOG_INFO("--------------------------------------------");
         SYSTEM_LOG_INFO("heapIndex: %d", memoryInfo.memoryTypes[i].heapIndex);
         SYSTEM_LOG_INFO("VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT: %s", memoryInfo.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: %s", memoryInfo.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: %s", memoryInfo.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_MEMORY_PROPERTY_HOST_CACHED_BIT: %s", memoryInfo.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT: %s", memoryInfo.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT ? "YES" : "NO");
      }
      SYSTEM_LOG_INFO("--------------------------------------------");
      SYSTEM_LOG_INFO("memoryHeapCount: %d", memoryInfo.memoryHeapCount);
      SYSTEM_LOG_INFO("Memory Heaps:");
      for (unsigned i = 0; i < memoryInfo.memoryHeapCount; ++i) {
         SYSTEM_LOG_INFO("size: %u MB", (unsigned)(memoryInfo.memoryHeaps[i].size / (1024LL* 1024LL)));
         SYSTEM_LOG_INFO("VK_MEMORY_HEAP_DEVICE_LOCAL_BIT: %s", memoryInfo.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "YES" : "NO");
         SYSTEM_LOG_INFO("VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX: %s", memoryInfo.memoryHeaps[i].flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX ? "YES" : "NO");
      }

      SYSTEM_LOG_INFO("--------------------------------------------");
      SYSTEM_LOG_INFO("FEATURE SET");
      SYSTEM_LOG_INFO("robustBufferAccess: %s", features.robustBufferAccess ? "YES" : "NO");
      SYSTEM_LOG_INFO("fullDrawIndexUint32: %s",features.fullDrawIndexUint32 ? "YES" : "NO");
      SYSTEM_LOG_INFO("imageCubeArray: %s",features.imageCubeArray ? "YES" : "NO");
      SYSTEM_LOG_INFO("independentBlend: %s",features.independentBlend ? "YES" : "NO");
      SYSTEM_LOG_INFO("geometryShader: %s",features.geometryShader ? "YES" : "NO");
      SYSTEM_LOG_INFO("tessellationShader: %s",features.tessellationShader ? "YES" : "NO");
      SYSTEM_LOG_INFO("sampleRateShading: %s",features.sampleRateShading ? "YES" : "NO");
      SYSTEM_LOG_INFO("dualSrcBlend: %s",features.dualSrcBlend ? "YES" : "NO");
      SYSTEM_LOG_INFO("logicOp: %s",features.logicOp ? "YES" : "NO");
      SYSTEM_LOG_INFO("multiDrawIndirect: %s",features.multiDrawIndirect ? "YES" : "NO");
      SYSTEM_LOG_INFO("drawIndirectFirstInstance: %s",features.drawIndirectFirstInstance ? "YES" : "NO");
      SYSTEM_LOG_INFO("depthClamp: %s",features.depthClamp ? "YES" : "NO");
      SYSTEM_LOG_INFO("depthBiasClamp: %s",features.depthBiasClamp ? "YES" : "NO");
      SYSTEM_LOG_INFO("fillModeNonSolid: %s",features.fillModeNonSolid ? "YES" : "NO");
      SYSTEM_LOG_INFO("depthBounds: %s",features.depthBounds ? "YES" : "NO");
      SYSTEM_LOG_INFO("wideLines: %s",features.wideLines ? "YES" : "NO");
      SYSTEM_LOG_INFO("largePoints: %s",features.largePoints ? "YES" : "NO");
      SYSTEM_LOG_INFO("alphaToOne: %s",features.alphaToOne ? "YES" : "NO");
      SYSTEM_LOG_INFO("multiViewport: %s",features.multiViewport ? "YES" : "NO");
      SYSTEM_LOG_INFO("samplerAnisotropy: %s",features.samplerAnisotropy ? "YES" : "NO");
      SYSTEM_LOG_INFO("textureCompressionETC2: %s",features.textureCompressionETC2 ? "YES" : "NO");
      SYSTEM_LOG_INFO("textureCompressionASTC_LDR: %s",features.textureCompressionASTC_LDR ? "YES" : "NO");
      SYSTEM_LOG_INFO("textureCompressionBC: %s",features.textureCompressionBC ? "YES" : "NO");
      SYSTEM_LOG_INFO("occlusionQueryPrecise: %s",features.occlusionQueryPrecise ? "YES" : "NO");
      SYSTEM_LOG_INFO("pipelineStatisticsQuery: %s",features.pipelineStatisticsQuery ? "YES" : "NO");
      SYSTEM_LOG_INFO("vertexPipelineStoresAndAtomics: %s",features.vertexPipelineStoresAndAtomics ? "YES" : "NO");
      SYSTEM_LOG_INFO("fragmentStoresAndAtomics: %s",features.fragmentStoresAndAtomics ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderTessellationAndGeometryPointSize: %s",features.shaderTessellationAndGeometryPointSize ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderImageGatherExtended: %s",features.shaderImageGatherExtended ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderStorageImageExtendedFormats: %s",features.shaderStorageImageExtendedFormats ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderStorageImageMultisample: %s",features.shaderStorageImageMultisample ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderStorageImageReadWithoutFormat: %s",features.shaderStorageImageReadWithoutFormat ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderStorageImageWriteWithoutFormat: %s",features.shaderStorageImageWriteWithoutFormat ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderUniformBufferArrayDynamicIndexing: %s",features.shaderUniformBufferArrayDynamicIndexing ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderSampledImageArrayDynamicIndexing: %s",features.shaderSampledImageArrayDynamicIndexing ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderStorageBufferArrayDynamicIndexing: %s",features.shaderStorageBufferArrayDynamicIndexing ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderStorageImageArrayDynamicIndexing: %s",features.shaderStorageImageArrayDynamicIndexing ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderClipDistance: %s",features.shaderClipDistance ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderCullDistance: %s",features.shaderCullDistance ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderFloat64: %s",features.shaderFloat64 ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderInt64: %s",features.shaderInt64 ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderInt16: %s",features.shaderInt16 ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderResourceResidency: %s",features.shaderResourceResidency ? "YES" : "NO");
      SYSTEM_LOG_INFO("shaderResourceMinLod: %s",features.shaderResourceMinLod ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseBinding: %s",features.sparseBinding ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidencyBuffer: %s",features.sparseResidencyBuffer ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidencyImage2D: %s",features.sparseResidencyImage2D ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidencyImage3D: %s",features.sparseResidencyImage3D ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidency2Samples: %s",features.sparseResidency2Samples ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidency4Samples: %s",features.sparseResidency4Samples ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidency8Samples: %s",features.sparseResidency8Samples ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidency16Samples: %s",features.sparseResidency16Samples ? "YES" : "NO");
      SYSTEM_LOG_INFO("sparseResidencyAliased: %s",features.sparseResidencyAliased ? "YES" : "NO");
      SYSTEM_LOG_INFO("variableMultisampleRate: %s",features.variableMultisampleRate ? "YES" : "NO");
      SYSTEM_LOG_INFO("inheritedQueries: %s",features.inheritedQueries ? "YES" : "NO");
      SYSTEM_LOG_INFO("--------------------------------------------");
      SYSTEM_LOG_INFO("SUPPORTED EXTENSIONS");

      //extensions
      for (auto & ext : extensions) {
         SYSTEM_LOG_INFO("%s", ext.c_str());
      }
   }
   
   //--------------------------------------------------------------------------
   //members
   VkInstance                       mVkInstance = nullptr;
   VkPhysicalDevice                 mPhysicalDevice = nullptr;
   VkDevice                         mVkDevice = nullptr;

   DevicePropertiesVulkan           mDeviceProperties{};
   RenderContextVulkanSettings      mSettings{};
   QueueFamilyIndex                 mQueueIndices{};
   QueueCreationInfo                mQueueConfiguration{};
   static std::atomic<unsigned>     mInstanceCount;
};

std::atomic<unsigned> RenderContextVulkan::mInstanceCount = 0;

}
}
