#include "..\include\VortexCore\RenderContext.h"
#include "..\include\VortexCore\AppWindow.h"
#include "..\include\VortexCore\private\RenderContextVulkan.h"
#include "..\include\VortexCore\private\SwapchainVulkan.h"

#include<string>


Vt::Gfx::RenderContext::RenderContext(const std::shared_ptr<Vt::App::AppWindow> &window, const RenderContextLayout &layout) :
   mWindow(window)
   , mLayout(layout)
   , mVkContext(nullptr) {
}


Vt::Gfx::RenderContext::~RenderContext() {

}

void Vt::Gfx::RenderContext::init() {
   if (!mVkContext) {
      RenderContextVulkanSettings settings;
#ifdef _DEBUG
      settings.mValidation = RenderContextVulkanSettings::ValidationFlags::STANDARD;
#else
      settings.mValidation = RenderContextVulkanSettings::ValidationFlags::NONE;
#endif
      try {
         //first we create a vulkan instance
         mVkContext = std::unique_ptr<RenderContextVulkan>(new RenderContextVulkan(*(mWindow.get()), settings));
         if (mVkContext) {
            //we find a suitable vulkan device
            auto physicalDevice = mVkContext->enumerateAndSelectDevice(Vt::Gfx::DeviceSelectionVulkan::AUTO_SELECT);
            //next we create a logical device
            VkDevice device{ nullptr };
            if (physicalDevice) {
               QueueCreationInfo queueCreateInfo{};
               queueCreateInfo.mGfxQueueCount = QueueCreationInfo::QueueCount::MAX;
               queueCreateInfo.mGfxQueueExclusive = false;
               queueCreateInfo.mComputeQueueCount = QueueCreationInfo::QueueCount::MAX;
               queueCreateInfo.mComputeQueueExclusive = true;
               queueCreateInfo.mTransferQueueCount = QueueCreationInfo::QueueCount::MAX;
               queueCreateInfo.mTransferQueueExclusive = true;

               //first we create a window surface, this is due to the device needing to create a
               //present queue based on the swapchain
               //For headless rendering we skip this
               createWindowSurface();
               
               //required extensions
               std::vector<std::string> requiredDeviceExtensions{
                  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                  VK_KHR_MAINTENANCE1_EXTENSION_NAME
               };

               //optional extensions
               std::vector<std::string> optionalDeviceExtensions{
                  VK_EXT_DEBUG_MARKER_EXTENSION_NAME
               };
               //then we create a device
               device = mVkContext->createDevice(physicalDevice, queueCreateInfo, requiredDeviceExtensions, optionalDeviceExtensions);
            }
            if (device) {
               //now we can init the swapchain
               if (!restoreSwapChain()) {
                  VT_EXCEPT(RenderContextException, "RenderContext::init: Could not restore swapchain!");
               }
            }
            //once we have a device we need to create command pools

         }
      } catch (const std::exception& e) {
         VT_EXCEPT_RT(RenderContextException, "RenderContext::init: Could not init render context!", e);
      }
   }
}

bool Vt::Gfx::RenderContext::swapBuffers() {
   auto sc(mSwapchain.lock());
   if (sc) {
      return sc->swapBuffers();
   }
   return false;
}

bool Vt::Gfx::RenderContext::toggleFullscreen() {
   return false;
}

std::weak_ptr<Vt::App::AppWindow> Vt::Gfx::RenderContext::window() const {
   return mWindow;
}

const Vt::Gfx::RenderContextLayout & Vt::Gfx::RenderContext::layout() const {
   return mLayout;
}

bool Vt::Gfx::RenderContext::createWindowSurface() {
   Vt::Gfx::SwapchainSettingsVulkan swapchainSettings{
      { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }, //format, colorspace
      { VK_PRESENT_MODE_IMMEDIATE_KHR }, //no v-sync
      { 0, 0 }, //window size - default
      { false }, //transfer target
      { VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR } //transform
   };
   mSwapchain = mVkContext->createWindowSurfaceAndSwapchain(swapchainSettings);
   return (mSwapchain.lock() != nullptr);
}

bool Vt::Gfx::RenderContext::restoreSwapChain() {
   auto sc = mSwapchain.lock();
   return sc->restore();;
}

void Vt::Gfx::RenderContext::resizeSwapChain(unsigned width, unsigned height) {
}
