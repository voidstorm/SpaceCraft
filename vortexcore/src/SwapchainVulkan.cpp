#include "..\include\VortexCore\private\RenderContextVulkan.h"
#include "..\include\VortexCore\private\SwapchainVulkan.h"
#include "..\include\VortexCore\AppWindow.h"
#include <thread>
#include <chrono>


//--------------------------------------------------------------------------
// Ctor, creates a vk instance

Vt::Gfx::SwapchainVulkan::SwapchainVulkan(const SwapchainSettingsVulkan & settings, RenderContextVulkan &context) :
   mSettings(settings),
   mContext(context){

}

//--------------------------------------------------------------------------
// D'tor

Vt::Gfx::SwapchainVulkan::~SwapchainVulkan() {
   if (mVkSurface) {
      vkDestroySurfaceKHR(mContext.vkInstance(), mVkSurface, nullptr);
      mVkSurface = nullptr;
      SYSTEM_LOG_INFO("Surface released!");
   }
}

//-----------------------------------------------------------------
//
VkSurfaceKHR Vt::Gfx::SwapchainVulkan::createSurface(const VkDevice device, const Vt::App::AppWindow & window) {

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

VkSurfaceKHR Vt::Gfx::SwapchainVulkan::getSurface() const {
   return mVkSurface;
}

bool Vt::Gfx::SwapchainVulkan::swapBuffers() {
   return false;
}

