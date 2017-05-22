#include "..\include\VortexCore\RenderContext.h"
#include "..\include\VortexCore\private\RenderContextVulkan.h"
#include<string>


Vt::Gfx::RenderContext::RenderContext(const std::shared_ptr<Vt::App::AppWindow> &window, const RenderContextLayout &layout) :
     mWindow(window)
   , mLayout(layout) 
   , mVkContext(nullptr){
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
         mVkContext = std::unique_ptr<RenderContextVulkan>(new RenderContextVulkan(settings));
         if (mVkContext) {
            //we find a suitable vulkan device
            auto physicalDevice= mVkContext->enumerateAndSelectDevice(Vt::Gfx::DeviceSelectionVulkan::AUTO_SELECT);
            //next we create a logical device
            if (physicalDevice) {
               QueueCreationInfo queueCreateInfo{};
               queueCreateInfo.mGfxQueueCount = QueueCreationInfo::QueueCount::MAX;
               queueCreateInfo.mGfxQueueExclusive = false;
               queueCreateInfo.mComputeQueueCount = QueueCreationInfo::QueueCount::MAX;
               queueCreateInfo.mComputeQueueExclusive = true;
               queueCreateInfo.mTransferQueueCount = QueueCreationInfo::QueueCount::MAX;
               queueCreateInfo.mTransferQueueExclusive = true;

               mVkContext->createDevice(physicalDevice, queueCreateInfo);
            }
            //once we have a device we need to create command pools

         }
      } catch (const std::exception& e) {
         VT_EXCEPT_RT(RenderContextException, "RenderContext::init: Could not init render context!", e);
      }
   }
}

bool Vt::Gfx::RenderContext::swapBuffers() {
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

bool Vt::Gfx::RenderContext::createSwapChain() {
   return false;
}

bool Vt::Gfx::RenderContext::restoreSwapChain() {
   return false;
}

void Vt::Gfx::RenderContext::resizeSwapChain(unsigned width, unsigned height) {
}
