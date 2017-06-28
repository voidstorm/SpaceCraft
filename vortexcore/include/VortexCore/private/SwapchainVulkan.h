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

class RenderContextVulkan;
struct SwapchainSettingsVulkan;

class SwapchainVkException : public std::runtime_error {
public:
   explicit SwapchainVkException(const std::string& what_arg) :std::runtime_error(what_arg) {};
   explicit SwapchainVkException(const char* what_arg) :std::runtime_error(what_arg) {};
};


class SwapchainVulkan {
   friend class RenderContext;
   friend class RenderContextVulkan;
   friend class std::unique_ptr<SwapchainVulkan>;
   friend struct std::unique_ptr<SwapchainVulkan>::deleter_type;
   
   friend class std::shared_ptr<SwapchainVulkan>;

   //--------------------------------------------------------------------------
   // Ctor, creates a vk instance
   SwapchainVulkan(const SwapchainSettingsVulkan &settings, RenderContextVulkan &context);

   //--------------------------------------------------------------------------
   // D'tor
   ~SwapchainVulkan();


   //-----------------------------------------------------------------
   // Creates a window surface for presentation
   VkSurfaceKHR createSurface(const Vt::App::AppWindow & window);

   //-----------------------------------------------------------------
   // Returns the window surface
   VkSurfaceKHR getSurface() const;

   //-----------------------------------------------------------------
   // swap buffers
   bool swapBuffers();
   
private:
   VkSurfaceKHR                     mVkSurface{ nullptr };
   SwapchainSettingsVulkan          mSettings;
   RenderContextVulkan              &mContext;
};

}
}
