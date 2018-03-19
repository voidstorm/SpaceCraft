#include "..\include\VortexCore\private\RenderContextVulkan.h"
#include "..\include\VortexCore\private\SwapchainVulkan.h"

#include "..\include\VortexCore\AppWindow.h"
#include <algorithm>

//Validation layers

static const char* VK_LAYER_LUNARG_api_dump = "VK_LAYER_LUNARG_api_dump"; 	//utility 	print API calls and their parameters and values
static const char* VK_LAYER_LUNARG_monitor = "VK_LAYER_LUNARG_monitor"; 	//utility 	outputs the frames - per - second of the target application in the applications title bar
static const char* VK_LAYER_GOOGLE_unique_objects = "VK_LAYER_GOOGLE_unique_objects";	//utility 	wrap all Vulkan objects in a unique pointer at create time and unwrap them at use time
static const char* VK_LAYER_LUNARG_core_validation = "VK_LAYER_LUNARG_core_validation";	//validation 	validate the descriptor set, pipeline state, and dynamic state; validate the interfaces between SPIR - V modules and the graphics pipeline; track and validate GPU memory and its binding to objects and command buffers; validate texture formats and render target formats
static const char* VK_LAYER_LUNARG_object_tracker = "VK_LAYER_LUNARG_object_tracker";	//validation 	track all Vulkan objects and flag invalid objects and object memory leaks
static const char* VK_LAYER_LUNARG_parameter_validation = "VK_LAYER_LUNARG_parameter_validation"; 	//validation 	validate API parameter values
static const char* VK_LAYER_LUNARG_screenshot = "VK_LAYER_LUNARG_screenshot"; 	//utility 	//outputs specified frames to an image file as they are presented
static const char* VK_LAYER_GOOGLE_threading = "VK_LAYER_GOOGLE_threading"; 	//validation 	//check validity of multi - threaded API usage
static const char* VK_LAYER_LUNARG_device_simulation = "VK_LAYER_LUNARG_device_simulation"; //
static const char* VK_LAYER_LUNARG_standard_validation = "VK_LAYER_LUNARG_standard_validation"; //


std::atomic<unsigned>  Vt::Gfx::RenderContextVulkan::mInstanceCount = 0;

PFN_vkCreateDebugReportCallbackEXT  Vt::Gfx::RenderContextVulkan::CreateDebugReportCallback = VK_NULL_HANDLE;
PFN_vkDestroyDebugReportCallbackEXT  Vt::Gfx::RenderContextVulkan::DestroyDebugReportCallback = VK_NULL_HANDLE;
PFN_vkDebugReportMessageEXT  Vt::Gfx::RenderContextVulkan::dbgBreakCallback = VK_NULL_HANDLE;
VkDebugReportCallbackEXT  Vt::Gfx::RenderContextVulkan::sMsgCallback = VK_NULL_HANDLE;

//--------------------------------------------------------------------------
// Ctor, creates a vk instance

Vt::Gfx::RenderContextVulkan::RenderContextVulkan(const Vt::App::AppWindow & window, const RenderContextVulkanSettings & settings) :
    mWindow(window)
   ,mContextSettings(settings) {
   mVkInstance = vkInstance();
   if (!mVkInstance) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::RenderContextVulkan: Could not create vulkan instance!");
   }
   mInstanceCount++;
}

//--------------------------------------------------------------------------
// D'tor

Vt::Gfx::RenderContextVulkan::~RenderContextVulkan() {
   mInstanceCount--;
   if (mInstanceCount == 0) {
      //release vulkan instance and device
      mSwapchain = nullptr;

      if (mVkDevice) {
         vkDestroyDevice(mVkDevice, nullptr);
         mVkDevice = nullptr;
         SYSTEM_LOG_INFO("Vulkan Device released!");
      }
      if (mVkInstance) {
         if (sMsgCallback != VK_NULL_HANDLE) {
            DestroyDebugReportCallback(mVkInstance, sMsgCallback, nullptr);
         }
         vkDestroyInstance(mVkInstance, nullptr);
         mVkInstance = nullptr;
         SYSTEM_LOG_INFO("Vulkan Instance released!");
      }
   }
}

//--------------------------------------------------------------------------
//retrieve vulkan instance

VkInstance Vt::Gfx::RenderContextVulkan::vkInstance() {
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
      std::vector<const char*> validationLayerNames;

      //debugging and validation support
      if (mContextSettings.mValidation == RenderContextVulkanSettings::ValidationFlags::STANDARD) {
          validationLayerNames.push_back(VK_LAYER_LUNARG_standard_validation);
          //validationLayerNames.push_back(VK_LAYER_LUNARG_monitor);
        
         instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
         instanceInfo.enabledLayerCount = (uint32_t)validationLayerNames.size();
         instanceInfo.ppEnabledLayerNames = validationLayerNames.data();
      } else if (mContextSettings.mValidation == RenderContextVulkanSettings::ValidationFlags::ALL) {
          validationLayerNames.push_back(VK_LAYER_LUNARG_standard_validation);
          //validationLayerNames.push_back(VK_LAYER_LUNARG_monitor);

         instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
         instanceInfo.enabledLayerCount = (uint32_t)validationLayerNames.size();
         instanceInfo.ppEnabledLayerNames = validationLayerNames.data();
      }

      if (instanceExtensions.size() > 0) {
         instanceInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
         instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
      }

      //create instance
      VkInstance instance = nullptr;
      VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &instance));


      //setup validation layer callbacks
      if (mContextSettings.mValidation > RenderContextVulkanSettings::ValidationFlags::NONE) {

         CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
         DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
         dbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));

         VkDebugReportCallbackCreateInfoEXT createInfo = {};
         createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
         createInfo.flags = mContextSettings.mValidation == RenderContextVulkanSettings::ValidationFlags::STANDARD ?
            VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT :
            VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
         createInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)&RenderContextVulkan::sValidationLayerCallback;
         createInfo.pUserData = reinterpret_cast<void*>(this);

         VK_CHECK_RESULT(CreateDebugReportCallback(instance, &createInfo, nullptr, &sMsgCallback));
      }

      SYSTEM_LOG_INFO("Created Instance with enabled layers:");
      for (int i = 0; i < (int)instanceInfo.enabledLayerCount; ++i) {
         SYSTEM_LOG_INFO("%s", instanceInfo.ppEnabledLayerNames[i]);
      }

      return instance;
   };

   static VkInstance instance = createInstance();
   return instance;
}

//-----------------------------------------------------------------
// Finds the proper queue family index
VkPhysicalDevice Vt::Gfx::RenderContextVulkan::vkPhysicalDevice() {
   return mPhysicalDevice;
}

//-----------------------------------------------------------------
// Finds the proper queue family index
VkDevice Vt::Gfx::RenderContextVulkan::vkDevice() {
   return mVkDevice;
}



//--------------------------------------------------------------------------
//enumerate and select devices

VkPhysicalDevice Vt::Gfx::RenderContextVulkan::enumerateAndSelectDevice(const DeviceSelectionVulkan device_selection) {
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
      SYSTEM_LOG_INFO("Device Id: 0x%x", deviceProperties.deviceID);
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
         uint32_t queueFamilyCount = 0;

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
            if (mDeviceProperties.mDeviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
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
   SYSTEM_LOG_INFO("Selected Adapter: %s Id:  0x%x", mDeviceProperties.mDeviceProperties.deviceName, mDeviceProperties.mDeviceProperties.deviceID);
   if (!mDeviceProperties.mQueueFamilyCount) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::enumerateAndSelectDevice: Could find a vulkan capable device!");
   }
   return mPhysicalDevice = selectedDevice;
}

uint32_t Vt::Gfx::RenderContextVulkan::findQueueFamilyIndex(const VkQueueFlagBits queueFlags) {
   // Dedicated queue for compute
   // Try to find a queue family index that supports compute but not graphics
   if (queueFlags & VK_QUEUE_COMPUTE_BIT) {
      for (uint32_t i = 0; i < static_cast<uint32_t>(mDeviceProperties.mDeviceQueueFamilyProperties.size()); i++) {
         if ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & queueFlags) && ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
            return i;
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
         }
      }
   }

   // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
   for (uint32_t i = 0; i < static_cast<uint32_t>(mDeviceProperties.mDeviceQueueFamilyProperties.size()); i++) {
      if (mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & queueFlags) {
         return i;
      }
   }
   SYSTEM_LOG_ERROR("RenderContextVulkan::getQueueFamilyIndex: Could not find a matching queue index!");
   VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::getQueueFamilyIndex: Could not find a matching queue index!");
   return 0;
}

//-----------------------------------------------------------------
//
uint32_t Vt::Gfx::RenderContextVulkan::findPresentQueueFamilyIndex() {
   // Try to find a queue family index that supports compute but not graphics
   int fall_back_device = -1;
   auto surface = mSwapchain->getSurface();
   if (!surface) {
      SYSTEM_LOG_ERROR("RenderContextVulkan::findPresentQueueFamilyIndex: No swapchain surface!");
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::findPresentQueueFamilyIndex: No swapchain surface!");
   }
   for (uint32_t i = 0; i < static_cast<uint32_t>(mDeviceProperties.mDeviceQueueFamilyProperties.size()); i++) {
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, surface, &presentSupport);
      if (presentSupport && ((mDeviceProperties.mDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)) {
         return i;
      } else if (presentSupport) {
         fall_back_device = i;
      }
   }
   return fall_back_device;
}

//-----------------------------------------------------------------
// Creates a logical device

VkDevice Vt::Gfx::RenderContextVulkan::createDevice(const VkPhysicalDevice device,
   const QueueCreationInfo & queueCreateInfo,
   const std::vector<std::string> &requiredDeviceExtensions,
   const std::vector<std::string> &optionalDeviceExtensions) {

   mQueueConfiguration = queueCreateInfo;
   std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

   // Get queue family indices for the requested queue family types
   // Note that the indices may overlap depending on the implementation

   const float defaultQueuePriorityGfx[(uint64_t)QueueCreationInfo::QueueCount::MAX] = {};
   const float defaultQueuePriorityCompute[(uint64_t)QueueCreationInfo::QueueCount::MAX] = {};
   const float defaultQueuePriorityTransfer[(uint64_t)QueueCreationInfo::QueueCount::MAX] = {};

   // Graphics queue
   if ((int)queueCreateInfo.mGfxQueueCount > 0) {
      mQueueIndices.mGraphics = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
      VkDeviceQueueCreateInfo queueInfo{};
      queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueInfo.queueFamilyIndex = mQueueIndices.mGraphics;
      queueInfo.queueCount = queueCreateInfo.mGfxQueueCount != QueueCreationInfo::QueueCount::MAX ? (int)queueCreateInfo.mGfxQueueCount :
         mDeviceProperties.mDeviceQueueFamilyProperties[mQueueIndices.mGraphics].queueCount;
      queueInfo.pQueuePriorities = defaultQueuePriorityGfx;
      queueCreateInfos.push_back(queueInfo);
      mQueueConfiguration.mGfxQueueCount = (QueueCreationInfo::QueueCount)queueInfo.queueCount;
   } else {
      mQueueIndices.mGraphics = VK_NULL_HANDLE;
      SYSTEM_LOG_ERROR("RenderContextVulkan::createDevice: Could not find a matching queue index for GfxQueue!");
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::createDevice: Could not find a matching queue index for GfxQueue!");
   }

   // Dedicated compute queue
   if ((int)queueCreateInfo.mComputeQueueCount > 0) {
      mQueueIndices.mCompute = findQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
      if (mQueueIndices.mCompute != mQueueIndices.mGraphics && queueCreateInfo.mComputeQueueExclusive) {
         // If compute family index differs, we need an additional queue create info for the compute queue
         VkDeviceQueueCreateInfo queueInfo{};
         queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
         queueInfo.queueFamilyIndex = mQueueIndices.mCompute;
         queueInfo.queueCount = queueCreateInfo.mComputeQueueCount != QueueCreationInfo::QueueCount::MAX ? (int)queueCreateInfo.mComputeQueueCount :
            mDeviceProperties.mDeviceQueueFamilyProperties[mQueueIndices.mCompute].queueCount;
         queueInfo.pQueuePriorities = defaultQueuePriorityCompute;
         queueCreateInfos.push_back(queueInfo);
         mQueueConfiguration.mComputeQueueCount = (QueueCreationInfo::QueueCount)queueInfo.queueCount;
         mQueueConfiguration.mComputeQueueExclusive = true;
      }
   } else if (mQueueIndices.mCompute == mQueueIndices.mGraphics && !queueCreateInfo.mComputeQueueExclusive) {
      // Else we use the same queue
      mQueueIndices.mCompute = mQueueIndices.mGraphics;
      mQueueConfiguration.mComputeQueueExclusive = false;
   } else {
      mQueueIndices.mCompute = mQueueIndices.mGraphics;
      mQueueConfiguration.mComputeQueueExclusive = false;
      SYSTEM_LOG_WARN("RenderContextVulkan::createDevice: Exclusive compute queue requested, but not available, using gfx queue instead!");
   }

   // Dedicated transfer queue
   if ((int)queueCreateInfo.mTransferQueueCount > 0) {
      mQueueIndices.mTransfer = findQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
      if (mQueueIndices.mTransfer != mQueueIndices.mGraphics && queueCreateInfo.mTransferQueueExclusive) {
         // If compute family index differs, we need an additional queue create info for the compute queue
         VkDeviceQueueCreateInfo queueInfo{};
         queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
         queueInfo.queueFamilyIndex = mQueueIndices.mTransfer;
         queueInfo.queueCount = queueCreateInfo.mTransferQueueCount != QueueCreationInfo::QueueCount::MAX ? (int)queueCreateInfo.mTransferQueueCount :
            mDeviceProperties.mDeviceQueueFamilyProperties[mQueueIndices.mTransfer].queueCount;
         queueInfo.pQueuePriorities = defaultQueuePriorityTransfer;
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

   //add present queue
   bool separatePresentQueue = false;
   for (auto & q : queueCreateInfos) {
      if (q.queueFamilyIndex == mQueueIndices.mPresent) {
         separatePresentQueue = false;
         break;
      } else {
         separatePresentQueue = true;
      }
   }
   if (separatePresentQueue) {
      VkDeviceQueueCreateInfo queueInfo{};
      queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueInfo.queueFamilyIndex = mQueueIndices.mPresent;
      queueInfo.queueCount = 1;
      queueInfo.pQueuePriorities = defaultQueuePriorityTransfer;
      queueCreateInfos.push_back(queueInfo);
      mQueueConfiguration.mPresentQueueExclusive = true;
   }

   //push all extensions
   std::vector<const char*> deviceExtensions;
   for (auto & ext : requiredDeviceExtensions) {
      if (std::any_of(std::begin(mDeviceProperties.mExtensions), std::end(mDeviceProperties.mExtensions), [&](const std::string &item)->bool {
         return item == ext;
      })) {
         SYSTEM_LOG_INFO("Loaded required extension: %s", ext.c_str());
         deviceExtensions.push_back(ext.c_str());
      } else {
         SYSTEM_LOG_ERROR("Extension required but not supported: %s", ext.c_str());
         VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::createDevice: vkCreateDevice failed!");
      }
   }

   for (auto & ext : optionalDeviceExtensions) {
      if (std::any_of(std::begin(mDeviceProperties.mExtensions), std::end(mDeviceProperties.mExtensions), [&](const std::string &item)->bool {
         return item == ext;
      })) {
         // Enable the debug marker extension if it is present (likely meaning a debugging tool is present)
         if (ext == std::string(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
            mDeviceProperties.mDebugMarkers = true;
         }
         SYSTEM_LOG_INFO("Loaded optional extension: %s", ext.c_str());
         deviceExtensions.push_back(ext.c_str());
      }
   }

   //we overwrite available extensions with the actual loaded extensions


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
   VkDevice logicalDevice = nullptr;
   VK_CHECK_RESULT(vkCreateDevice(device, &deviceCreateInfo, nullptr, &logicalDevice));

   if (!logicalDevice) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::createDevice: vkCreateDevice failed!");
   }

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
   SYSTEM_LOG_INFO("Present Queue Idx: %d", mQueueIndices.mPresent);
   SYSTEM_LOG_INFO("Present Queue Exclusive: %s", mQueueConfiguration.mPresentQueueExclusive ? "YES" : "NO");

   return mVkDevice = logicalDevice;
}

//-----------------------------------------------------------------
//
std::weak_ptr<Vt::Gfx::SwapchainVulkan> Vt::Gfx::RenderContextVulkan::createWindowSurfaceAndSwapchain(const SwapchainSettingsVulkan & swapchainSettings) {
   mSwapchain = std::unique_ptr<Vt::Gfx::SwapchainVulkan>(new SwapchainVulkan(mWindow, swapchainSettings, *this));
   //create window surface
   mSwapchain->createSurface();
   //find and set a proper present queue
   mQueueIndices.mPresent = findPresentQueueFamilyIndex();
   return mSwapchain;
}

//-----------------------------------------------------------------
//log some useful adapter info

void Vt::Gfx::RenderContextVulkan::logAdapterProperties(const VkPhysicalDeviceProperties & deviceProperties, const std::vector<VkQueueFamilyProperties>& queueProperties, const VkPhysicalDeviceFeatures & features, const VkPhysicalDeviceMemoryProperties & memoryInfo, const std::vector<std::string>& extensions) {

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
      SYSTEM_LOG_INFO("size: %u MB", (unsigned)(memoryInfo.memoryHeaps[i].size / (1024LL * 1024LL)));
      SYSTEM_LOG_INFO("VK_MEMORY_HEAP_DEVICE_LOCAL_BIT: %s", memoryInfo.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "YES" : "NO");
      SYSTEM_LOG_INFO("VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX: %s", memoryInfo.memoryHeaps[i].flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX ? "YES" : "NO");
   }

   SYSTEM_LOG_INFO("--------------------------------------------");
   SYSTEM_LOG_INFO("FEATURE SET");
   SYSTEM_LOG_INFO("robustBufferAccess: %s", features.robustBufferAccess ? "YES" : "NO");
   SYSTEM_LOG_INFO("fullDrawIndexUint32: %s", features.fullDrawIndexUint32 ? "YES" : "NO");
   SYSTEM_LOG_INFO("imageCubeArray: %s", features.imageCubeArray ? "YES" : "NO");
   SYSTEM_LOG_INFO("independentBlend: %s", features.independentBlend ? "YES" : "NO");
   SYSTEM_LOG_INFO("geometryShader: %s", features.geometryShader ? "YES" : "NO");
   SYSTEM_LOG_INFO("tessellationShader: %s", features.tessellationShader ? "YES" : "NO");
   SYSTEM_LOG_INFO("sampleRateShading: %s", features.sampleRateShading ? "YES" : "NO");
   SYSTEM_LOG_INFO("dualSrcBlend: %s", features.dualSrcBlend ? "YES" : "NO");
   SYSTEM_LOG_INFO("logicOp: %s", features.logicOp ? "YES" : "NO");
   SYSTEM_LOG_INFO("multiDrawIndirect: %s", features.multiDrawIndirect ? "YES" : "NO");
   SYSTEM_LOG_INFO("drawIndirectFirstInstance: %s", features.drawIndirectFirstInstance ? "YES" : "NO");
   SYSTEM_LOG_INFO("depthClamp: %s", features.depthClamp ? "YES" : "NO");
   SYSTEM_LOG_INFO("depthBiasClamp: %s", features.depthBiasClamp ? "YES" : "NO");
   SYSTEM_LOG_INFO("fillModeNonSolid: %s", features.fillModeNonSolid ? "YES" : "NO");
   SYSTEM_LOG_INFO("depthBounds: %s", features.depthBounds ? "YES" : "NO");
   SYSTEM_LOG_INFO("wideLines: %s", features.wideLines ? "YES" : "NO");
   SYSTEM_LOG_INFO("largePoints: %s", features.largePoints ? "YES" : "NO");
   SYSTEM_LOG_INFO("alphaToOne: %s", features.alphaToOne ? "YES" : "NO");
   SYSTEM_LOG_INFO("multiViewport: %s", features.multiViewport ? "YES" : "NO");
   SYSTEM_LOG_INFO("samplerAnisotropy: %s", features.samplerAnisotropy ? "YES" : "NO");
   SYSTEM_LOG_INFO("textureCompressionETC2: %s", features.textureCompressionETC2 ? "YES" : "NO");
   SYSTEM_LOG_INFO("textureCompressionASTC_LDR: %s", features.textureCompressionASTC_LDR ? "YES" : "NO");
   SYSTEM_LOG_INFO("textureCompressionBC: %s", features.textureCompressionBC ? "YES" : "NO");
   SYSTEM_LOG_INFO("occlusionQueryPrecise: %s", features.occlusionQueryPrecise ? "YES" : "NO");
   SYSTEM_LOG_INFO("pipelineStatisticsQuery: %s", features.pipelineStatisticsQuery ? "YES" : "NO");
   SYSTEM_LOG_INFO("vertexPipelineStoresAndAtomics: %s", features.vertexPipelineStoresAndAtomics ? "YES" : "NO");
   SYSTEM_LOG_INFO("fragmentStoresAndAtomics: %s", features.fragmentStoresAndAtomics ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderTessellationAndGeometryPointSize: %s", features.shaderTessellationAndGeometryPointSize ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderImageGatherExtended: %s", features.shaderImageGatherExtended ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderStorageImageExtendedFormats: %s", features.shaderStorageImageExtendedFormats ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderStorageImageMultisample: %s", features.shaderStorageImageMultisample ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderStorageImageReadWithoutFormat: %s", features.shaderStorageImageReadWithoutFormat ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderStorageImageWriteWithoutFormat: %s", features.shaderStorageImageWriteWithoutFormat ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderUniformBufferArrayDynamicIndexing: %s", features.shaderUniformBufferArrayDynamicIndexing ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderSampledImageArrayDynamicIndexing: %s", features.shaderSampledImageArrayDynamicIndexing ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderStorageBufferArrayDynamicIndexing: %s", features.shaderStorageBufferArrayDynamicIndexing ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderStorageImageArrayDynamicIndexing: %s", features.shaderStorageImageArrayDynamicIndexing ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderClipDistance: %s", features.shaderClipDistance ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderCullDistance: %s", features.shaderCullDistance ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderFloat64: %s", features.shaderFloat64 ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderInt64: %s", features.shaderInt64 ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderInt16: %s", features.shaderInt16 ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderResourceResidency: %s", features.shaderResourceResidency ? "YES" : "NO");
   SYSTEM_LOG_INFO("shaderResourceMinLod: %s", features.shaderResourceMinLod ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseBinding: %s", features.sparseBinding ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidencyBuffer: %s", features.sparseResidencyBuffer ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidencyImage2D: %s", features.sparseResidencyImage2D ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidencyImage3D: %s", features.sparseResidencyImage3D ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidency2Samples: %s", features.sparseResidency2Samples ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidency4Samples: %s", features.sparseResidency4Samples ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidency8Samples: %s", features.sparseResidency8Samples ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidency16Samples: %s", features.sparseResidency16Samples ? "YES" : "NO");
   SYSTEM_LOG_INFO("sparseResidencyAliased: %s", features.sparseResidencyAliased ? "YES" : "NO");
   SYSTEM_LOG_INFO("variableMultisampleRate: %s", features.variableMultisampleRate ? "YES" : "NO");
   SYSTEM_LOG_INFO("inheritedQueries: %s", features.inheritedQueries ? "YES" : "NO");
   SYSTEM_LOG_INFO("--------------------------------------------");
   SYSTEM_LOG_INFO("SUPPORTED EXTENSIONS");

   //extensions
   for (auto & ext : extensions) {
      SYSTEM_LOG_INFO("%s", ext.c_str());
   }
}

//--------------------------------------------------------------------------
//
uint32_t Vt::Gfx::RenderContextVulkan::queueFamilyIndex(Vt::Gfx::QueueType type) const {
   switch (type) {
      case Vt::Gfx::QueueType::GRAPHICS:
         return mQueueIndices.mGraphics;
         break;
      case Vt::Gfx::QueueType::COMPUTE:
         return mQueueIndices.mCompute;
         break;
      case Vt::Gfx::QueueType::TRANSFER:
         return mQueueIndices.mTransfer;
         break;
      case Vt::Gfx::QueueType::PRESENT:
         return mQueueIndices.mPresent;
         break;
      default:
         break;
   }
   return 0;
}

//--------------------------------------------------------------------------
//
uint32_t Vt::Gfx::RenderContextVulkan::queueCount(QueueType type) const {
   switch (type) {
      case Vt::Gfx::QueueType::GRAPHICS:
         return (uint32_t)mQueueConfiguration.mGfxQueueCount;
         break;
      case Vt::Gfx::QueueType::COMPUTE:
         return (uint32_t)mQueueConfiguration.mComputeQueueCount;
         break;
      case Vt::Gfx::QueueType::TRANSFER:
         return (uint32_t)mQueueConfiguration.mTransferQueueCount;
         break;
      default:
         break;
   }
   return 0;
}

//--------------------------------------------------------------------------
Vt::Gfx::QueueCreationInfo const & Vt::Gfx::RenderContextVulkan::queueConfiguration() const {
   return mQueueConfiguration;
}

//--------------------------------------------------------------------------
//
VkQueue Vt::Gfx::RenderContextVulkan::deviceQueue(QueueType type, uint32_t index) const {
   VkQueue queue = nullptr;
   uint32_t family_idx = queueFamilyIndex(type);
   if (index > queueCount(type)) {
      SYSTEM_LOG_ERROR("RenderContextVulkan::getDeviceQueue: index > queue count!");
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::getDeviceQueue: index > queue count!");
   }
   vkGetDeviceQueue(mVkDevice, family_idx, index, &queue);
   return queue;
}

//--------------------------------------------------------------------------
bool Vt::Gfx::RenderContextVulkan::checkInstanceExtension(const std::string & extension) {
   return false;
}

//--------------------------------------------------------------------------
bool Vt::Gfx::RenderContextVulkan::checkDeviceExtension(const std::string & extension) {
   for (auto & ext : mDeviceProperties.mExtensions) {
      if (ext == extension) {
         return true;
      }
   }
   return false;
}

//--------------------------------------------------------------------------
VkPhysicalDeviceFeatures Vt::Gfx::RenderContextVulkan::deviceFeatures() const {
   return mDeviceProperties.mDeviceFeatures;
}


//--------------------------------------------------------------------------
//members

VKAPI_ATTR VkBool32 VKAPI_CALL Vt::Gfx::RenderContextVulkan::validationLayerCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char * layerPrefix, const char * msg) {

   // Error that may result in undefined behaviour
   if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
      SYSTEM_LOG_ERROR("%s, code: %d, msg: %s", layerPrefix, code, msg);
   };
   // Warnings may hint at unexpected / non-spec API usage
   if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
      SYSTEM_LOG_WARN("%s, code: %d, msg: %s", layerPrefix, code, msg);
   };
   // May indicate sub-optimal usage of the API
   if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
      SYSTEM_LOG_WARN("%s, code: %d, msg: %s", layerPrefix, code, msg);
   };
   // Informal messages that may become handy during debugging
   if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
      SYSTEM_LOG_INFO("%s, code: %d, msg: %s", layerPrefix, code, msg);
   }
   // Diagnostic info from the Vulkan loader and layers
   // Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
   if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
      SYSTEM_LOG_INFO("%s, code: %d, msg: %s", layerPrefix, code, msg);
   }
   return VK_TRUE;
}

//--------------------------------------------------------------------------
//members

VKAPI_ATTR VkBool32 VKAPI_CALL Vt::Gfx::RenderContextVulkan::sValidationLayerCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char * layerPrefix, const char * msg, void * userData) {
   return reinterpret_cast<Vt::Gfx::RenderContextVulkan*>(userData)->validationLayerCallback(flags,
      objType,
      obj,
      location,
      code,
      layerPrefix,
      msg);
}
