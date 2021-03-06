#include "..\include\VortexCore\private\RenderContextVulkan.h"
#include "..\include\VortexCore\private\SwapchainVulkan.h"
#include "..\include\VortexCore\AppWindow.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <limits>

#ifdef max 
#undef max
#undef min
#endif

//--------------------------------------------------------------------------
// Ctor, creates a vk instance

Vt::Gfx::SwapchainVulkan::SwapchainVulkan(const Vt::App::AppWindow & window, const SwapchainSettingsVulkan & settings, RenderContextVulkan &context) :
   mWindow(window),
   mSettings(settings),
   mContext(context) {

}

//--------------------------------------------------------------------------
// D'tor

Vt::Gfx::SwapchainVulkan::~SwapchainVulkan() {
   releaseImageViews();
   releaseSwapchain();
   releaseSurface();
}

//-----------------------------------------------------------------
//
VkSurfaceKHR Vt::Gfx::SwapchainVulkan::createSurface() {

   VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
   surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
   surfaceCreateInfo.hinstance = (HINSTANCE)mWindow.instance();
   surfaceCreateInfo.hwnd = mWindow.winId();
   VkSurfaceKHR surface{ nullptr };
   VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(mContext.vkInstance(), &surfaceCreateInfo, nullptr, &surface));
   if (!surface) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::createSurface: vkCreateWin32SurfaceKHR failed!");
   }
   return mVkSurface = surface;
}


//-----------------------------------------------------------------
//
VkSurfaceKHR Vt::Gfx::SwapchainVulkan::getSurface() const {
   return mVkSurface;
}

//-----------------------------------------------------------------
//
void Vt::Gfx::SwapchainVulkan::releaseSurface() {
   if (mVkSurface) {
      vkDestroySurfaceKHR(mContext.vkInstance(), mVkSurface, nullptr);
      mVkSurface = nullptr;
      SYSTEM_LOG_INFO("Surface released!");
   }
}

//-----------------------------------------------------------------
//
void Vt::Gfx::SwapchainVulkan::releaseSwapchain() {
   if (mVkSwapchain) {
      vkDestroySwapchainKHR(mContext.vkDevice(), mVkSwapchain, nullptr);
      mVkSwapchain = nullptr;
      SYSTEM_LOG_INFO("Swapchain released!");
   }
}

//-----------------------------------------------------------------
VkSurfaceFormatKHR Vt::Gfx::SwapchainVulkan::chooseSwapchainFormat() {
   //auto choose
   if (mSettings.mSurfaceFormat.format == VK_FORMAT_UNDEFINED) {
      if (mProperties.mFormats.size() == 1 && mProperties.mFormats[0].format == VK_FORMAT_UNDEFINED) {
         return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
      }

      for (const auto& availableFormat : mProperties.mFormats) {
         if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
         }
      }
      //otherwise we return the first available format
      return mProperties.mFormats[0];
   } else {
      //use the format that was specified
      for (const auto& availableFormat : mProperties.mFormats) {
         if (availableFormat.format == mSettings.mSurfaceFormat.format && availableFormat.colorSpace == mSettings.mSurfaceFormat.colorSpace) {
            return availableFormat;
         }
      }
      //else we have a problem
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::chooseSwapchainFormat: Requested swapchain surface format not available!");
   }
   return VkSurfaceFormatKHR{};
}

//-----------------------------------------------------------------
VkPresentModeKHR Vt::Gfx::SwapchainVulkan::choosePresentMode() {
   VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

   for (const auto& availablePresentMode : mProperties.mPresentModes) {
      if (availablePresentMode == mSettings.mPresentMode) {
         return availablePresentMode;
      }
   }
   SYSTEM_LOG_WARN("RenderContextVulkan::choosePresentMode: Requested present mode (%d) not avail, using fallback!", (int)mSettings.mPresentMode);
   //if not avail use fallback
   for (const auto& availablePresentMode : mProperties.mPresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
         return availablePresentMode;
      } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
         bestMode = availablePresentMode;
      }
   }
   return bestMode;
}

//-----------------------------------------------------------------
// size of swap surface
VkExtent2D Vt::Gfx::SwapchainVulkan::chooseSwapExtent() {
   if (mProperties.mCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      return mProperties.mCapabilities.currentExtent;
   } else {
      VkExtent2D actualExtent = { mSettings.mSize };

      actualExtent.width = std::max(mProperties.mCapabilities.minImageExtent.width, std::min(mProperties.mCapabilities.maxImageExtent.width, actualExtent.width));
      actualExtent.height = std::max(mProperties.mCapabilities.minImageExtent.height, std::min(mProperties.mCapabilities.maxImageExtent.height, actualExtent.height));

      return actualExtent;
   }
}
 
//-----------------------------------------------------------------
// create the swapchain
VkSwapchainKHR Vt::Gfx::SwapchainVulkan::createSwapchain() {
   VkSwapchainKHR swapchain = nullptr;
   //first we determine the lenght of the swapchain queue
   uint32_t imageCount = mProperties.mCapabilities.minImageCount + 1;
   if (mProperties.mCapabilities.maxImageCount > 0 && imageCount > mProperties.mCapabilities.maxImageCount) {
      imageCount = mProperties.mCapabilities.maxImageCount;
   }

   VkSwapchainCreateInfoKHR createInfo = {};
   createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
   createInfo.surface = mVkSurface;

   createInfo.minImageCount = imageCount;
   createInfo.imageFormat = mSettings.mSurfaceFormat.format;
   createInfo.imageColorSpace = mSettings.mSurfaceFormat.colorSpace;
   createInfo.imageExtent = mSettings.mSize;
   createInfo.imageArrayLayers = 1; //set to > 1 if stereoscopic
   createInfo.imageUsage = mSettings.mFbTransferTarget == false ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_TRANSFER_DST_BIT;

   uint32_t queueFamilyIndices[] = { mContext.queueFamilyIndex(Vt::Gfx::QueueType::GRAPHICS), mContext.queueFamilyIndex(Vt::Gfx::QueueType::PRESENT) };

   if (mContext.queueConfiguration().mPresentQueueExclusive) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
   } else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0; // Optional
      createInfo.pQueueFamilyIndices = nullptr; // Optional
   }
   //e.g rotate image
   createInfo.preTransform = mSettings.mTransform == VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? mProperties.mCapabilities.currentTransform : mSettings.mTransform;
   createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
   createInfo.presentMode = mSettings.mPresentMode;
   createInfo.clipped = VK_TRUE;
   createInfo.oldSwapchain = mVkSwapchain;

   // Set additional usage flag for blitting from the swapchain images if supported
   VkFormatProperties formatProps;
   vkGetPhysicalDeviceFormatProperties(mContext.vkPhysicalDevice(), createInfo.imageFormat, &formatProps);
   if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) && mSettings.mSupportBlit) {
      createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
   }

   VK_CHECK_RESULT(vkCreateSwapchainKHR(mContext.vkDevice(), &createInfo, nullptr, &swapchain));
   VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mContext.vkDevice(), swapchain, &imageCount, nullptr));
   mSwapChainImages.resize(imageCount);
   VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mContext.vkDevice(), swapchain, &imageCount, mSwapChainImages.data()));

   return swapchain;
}

//-----------------------------------------------------------------
// query swapchain properties
bool Vt::Gfx::SwapchainVulkan::createImageViews() {
   mSwapChainImageViews.resize(mSwapChainImages.size());
   auto view = 0;
   for (const auto & img : mSwapChainImages) {
      VkImageViewCreateInfo createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      createInfo.image = img;
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      createInfo.format = mSettings.mSurfaceFormat.format;
      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      VK_CHECK_RESULT(vkCreateImageView(mContext.vkDevice(), &createInfo, nullptr, &mSwapChainImageViews[view++]));
   }
   return true;
}

//-----------------------------------------------------------------
// query swapchain properties
void Vt::Gfx::SwapchainVulkan::releaseImageViews() {
   for (auto & img : mSwapChainImageViews) {
      vkDestroyImageView(mContext.vkDevice(), img, nullptr);
   }
}

//-----------------------------------------------------------------
// query swapchain properties
void Vt::Gfx::SwapchainVulkan::logSwapchainProperties() {
   SYSTEM_LOG_INFO("--------------------------------------------");
   SYSTEM_LOG_INFO("Swapchain Properties");
   SYSTEM_LOG_INFO("--------------------------------------------");
   SYSTEM_LOG_INFO("minImageCount: %d", mProperties.mCapabilities.minImageCount);
   SYSTEM_LOG_INFO("maxImageCount: %d", mProperties.mCapabilities.maxImageCount);
   SYSTEM_LOG_INFO("currentExtent: %dx%d", mProperties.mCapabilities.currentExtent.width, mProperties.mCapabilities.currentExtent.height);
   SYSTEM_LOG_INFO("minImageExtent: %dx%d", mProperties.mCapabilities.minImageExtent.width, mProperties.mCapabilities.minImageExtent.height);
   SYSTEM_LOG_INFO("maxImageExtent: %dx%d", mProperties.mCapabilities.maxImageExtent.width, mProperties.mCapabilities.maxImageExtent.height);
   SYSTEM_LOG_INFO("maxImageArrayLayers: %d", mProperties.mCapabilities.maxImageArrayLayers);
   SYSTEM_LOG_INFO("--------------------------------------------");
   SYSTEM_LOG_INFO("Swapchain Created with:");
   SYSTEM_LOG_INFO("--------------------------------------------");
   SYSTEM_LOG_INFO("Swapchain image count: %d", mSwapChainImages.size());
   SYSTEM_LOG_INFO("Width: %d", mSettings.mSize.width);
   SYSTEM_LOG_INFO("Height: %d", mSettings.mSize.height);
   SYSTEM_LOG_INFO("Surface format: 0x%x", (uint32_t)mSettings.mSurfaceFormat.format);
   SYSTEM_LOG_INFO("Color space format: 0x%x", (uint32_t)mSettings.mSurfaceFormat.colorSpace);
   SYSTEM_LOG_INFO("Present mode: 0x%x", (uint32_t)mSettings.mPresentMode);
   SYSTEM_LOG_INFO("Transfer target: %s", mSettings.mFbTransferTarget == true ? "YES" : "NO");
   SYSTEM_LOG_INFO("Transform flags: 0x%x", (uint32_t)mSettings.mTransform);
}

//-----------------------------------------------------------------
// query swapchain properties
Vt::Gfx::SwapchainPropertiesVulkan const & Vt::Gfx::SwapchainVulkan::querySwapchainProperties() {
   if (mProperties.mFormats.empty()) {
      //surf caps
      VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mContext.vkPhysicalDevice(), mVkSurface, &mProperties.mCapabilities));

      //surf formats
      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(mContext.vkPhysicalDevice(), mVkSurface, &formatCount, nullptr);
      if (formatCount != 0) {
         mProperties.mFormats.resize(formatCount);
         VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mContext.vkPhysicalDevice(), mVkSurface, &formatCount, mProperties.mFormats.data()));
      }

      //present modes
      uint32_t presentModeCount;
      VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext.vkPhysicalDevice(), mVkSurface, &presentModeCount, nullptr));

      if (presentModeCount != 0) {
         mProperties.mPresentModes.resize(presentModeCount);
         VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext.vkPhysicalDevice(), mVkSurface, &presentModeCount, mProperties.mPresentModes.data()));
      }
   }
   return mProperties;
}

//-----------------------------------------------------------------
//
bool Vt::Gfx::SwapchainVulkan::isDeviceSuitable(const std::vector<VkSurfaceFormatKHR> formats, const std::vector<VkPresentModeKHR> presentModes) {
   return !(mProperties.mFormats.empty() && mProperties.mPresentModes.empty());
}

//-----------------------------------------------------------------
//
bool Vt::Gfx::SwapchainVulkan::restore() {
   bool status = true;
   if (!mContext.checkDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::restore: VK_KHR_SWAPCHAIN_EXTENSION not supported!");
      return false;
   }
   releaseImageViews();
   releaseSwapchain();
   //re-create swapchain
   querySwapchainProperties();

   //check if we can create a swapchain with the requested settings
   //Right now this is not implemented
   if (!isDeviceSuitable()) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::restore: Device not suitable for creating the requested swapchain!");
      return false;
   }

   //choose swapchain surface format
   mSettings.mSurfaceFormat = chooseSwapchainFormat();

   //choose presentation mode
   mSettings.mPresentMode = choosePresentMode();

   //swap extent
   mSettings.mSize = chooseSwapExtent();

   //now we can create our swapchain
   status &= ((mVkSwapchain = createSwapchain()) != nullptr);

   //create image views, if swapchain is used as render target
   if (!mSettings.mFbTransferTarget) {
      status &= createImageViews();
   }

   //write some logs
   logSwapchainProperties();
   return status;
}

//-----------------------------------------------------------------
//
bool Vt::Gfx::SwapchainVulkan::swapBuffers() {
   //mWindow.lock();
   //mWindow.unlock();
   return false;
}

//-----------------------------------------------------------------
Vt::Gfx::SwapchainSettingsVulkan Vt::Gfx::SwapchainVulkan::settings() {
   return mSettings;
}

