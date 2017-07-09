#include "..\include\VortexCore\private\RenderContextVulkan.h"
#include "..\include\VortexCore\private\SwapchainVulkan.h"
#include "..\include\VortexCore\AppWindow.h"
#include <thread>
#include <chrono>
#include <algorithm>


//--------------------------------------------------------------------------
// Ctor, creates a vk instance

Vt::Gfx::SwapchainVulkan::SwapchainVulkan(const SwapchainSettingsVulkan & settings, RenderContextVulkan &context) :
   mSettings(settings),
   mContext(context){

}

//--------------------------------------------------------------------------
// D'tor

Vt::Gfx::SwapchainVulkan::~SwapchainVulkan() {
   releaseSurface();
}

//-----------------------------------------------------------------
//
VkSurfaceKHR Vt::Gfx::SwapchainVulkan::createSurface(const Vt::App::AppWindow & window) {

   VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
   surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
   surfaceCreateInfo.hinstance = (HINSTANCE)window.instance();
   surfaceCreateInfo.hwnd = window.winId();
   VkSurfaceKHR surface{ nullptr };
   VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(mContext.vkInstance(), &surfaceCreateInfo, nullptr, &surface));
   if (!surface) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::createSurface: vkCreateWin32SurfaceKHR failed!");
   }
   return mVkSurface= surface;
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
   if (!mContext.checkDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
      VT_EXCEPT(RenderContextVkException, "RenderContextVulkan::restore: VK_KHR_SWAPCHAIN_EXTENSION not supported!");
      return false;
   }
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
   mSettings.mSurfaceFormat= chooseSwapchainFormat();

   //choose presentation mode
   mSettings.mPresentMode = choosePresentMode();

   return true;
}

//-----------------------------------------------------------------
//
bool Vt::Gfx::SwapchainVulkan::swapBuffers() {

   return false;
}

//-----------------------------------------------------------------
Vt::Gfx::SwapchainSettingsVulkan Vt::Gfx::SwapchainVulkan::settings() {
   return mSettings;
}

