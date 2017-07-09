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

struct SwapchainPropertiesVulkan {
   VkSurfaceCapabilitiesKHR mCapabilities;
   std::vector<VkSurfaceFormatKHR> mFormats;
   std::vector<VkPresentModeKHR> mPresentModes;
};

class SwapchainVkException : public std::runtime_error {
public:
   explicit SwapchainVkException(const std::string& what_arg) :std::runtime_error(what_arg) {};
   explicit SwapchainVkException(const char* what_arg) :std::runtime_error(what_arg) {};
};


class SwapchainVulkan {
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
   // Releases the window surface
   void releaseSurface();

   //-----------------------------------------------------------------
   // Releases the swapchain
   void releaseSwapchain();


   //-----------------------------------------------------------------
   // Chooses the swapchain format based on settings
   VkSurfaceFormatKHR  chooseSwapchainFormat();

   //-----------------------------------------------------------------
   // Chooses present mode based on settings
   VkPresentModeKHR choosePresentMode();

public:

   //-----------------------------------------------------------------
   // Queries all swapchain properties
   SwapchainPropertiesVulkan const & querySwapchainProperties();

   //-----------------------------------------------------------------
   // Checks if the device supports the specified formats and modes
   // If none is specified it returns true if at least one format and mode
   // is available
   // Note, that right now checking of device formats is not implemented
   bool isDeviceSuitable(const std::vector<VkSurfaceFormatKHR> formats = std::vector<VkSurfaceFormatKHR>{}, 
                         const std::vector<VkPresentModeKHR> presentModes = std::vector<VkPresentModeKHR>{});

   //-----------------------------------------------------------------
   // Restores the swapchain
   bool restore();

   //-----------------------------------------------------------------
   // swap buffers
   bool swapBuffers();

   //-----------------------------------------------------------------
   SwapchainSettingsVulkan settings();

private:
   VkSurfaceKHR                     mVkSurface{ nullptr };
   SwapchainSettingsVulkan          mSettings{};
   RenderContextVulkan              &mContext;
   SwapchainPropertiesVulkan        mProperties{};
};

}
}
