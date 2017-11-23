#pragma once
#include <memory>
#include <vector>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include "vulkan\vulkan.h"


#include "..\SystemLogger.h"
#include "VkErrorHelper.h"

namespace Vt {
namespace Gfx {

class RenderContextVulkan;
class ShaderVulkan final {
public:
   //-----------------------------------------------------------------
   // Loads the shader file into a buffer
   ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan &ctx);
   ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan &ctx, const std::vector<char> &buffer);
   ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan &ctx, std::ifstream& stream);
   ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan &ctx, const std::string& filename);

   //-----------------------------------------------------------------
   // Returns shader module
   VkShaderModule module() const;

   //-----------------------------------------------------------------
   // Returns the pipeline stage info for attaching the shader to a pipeline
   VkPipelineShaderStageCreateInfo stage() const;

   ~ShaderVulkan();
private:
   //-----------------------------------------------------------------
   // Creates the shader module
   void createModule();

   std::vector<char> mData;
   VkShaderModule mModule{ nullptr };
   RenderContextVulkan &mCtx;
   VkShaderStageFlagBits mStage;
};
}
}
