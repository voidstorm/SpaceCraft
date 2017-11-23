#include "..\include\VortexCore\private\ShaderVulkan.h"
#include "..\include\VortexCore\private\RenderContextVulkan.h"


Vt::Gfx::ShaderVulkan::ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan & ctx)
   : mCtx(ctx)
   , mStage(stage)
   {
}

Vt::Gfx::ShaderVulkan::ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan &ctx, const std::vector<char>& buffer)
   :
   ShaderVulkan(stage, ctx){
   mData = buffer;
   if (mData.empty()) {
      SYSTEM_LOG_ERROR("ShaderVulkan::ShaderVulkan: File empty!");
   }
 
   createModule();
}

//-----------------------------------------------------------------
//
Vt::Gfx::ShaderVulkan::ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan &ctx, std::ifstream & stream)
   :
   ShaderVulkan(stage, ctx) {
   if (!stream.is_open()) {
      VT_EXCEPT(std::runtime_error , "ShaderVulkan::ShaderVulkan: Could not open stream!");
   }

   size_t fileSize = (size_t)stream.tellg();

   if (fileSize == 0) {
      SYSTEM_LOG_ERROR("ShaderVulkan::ShaderVulkan: File empty!");
   }

   mData.resize(fileSize);
   stream.seekg(0);
   stream.read(mData.data(), fileSize);

   createModule();
}

//-----------------------------------------------------------------
//
Vt::Gfx::ShaderVulkan::ShaderVulkan(VkShaderStageFlagBits stage, RenderContextVulkan &ctx, const std::string & filename)
   :
   ShaderVulkan(stage, ctx) {
   std::ifstream stream(filename, std::ios::ate | std::ios::binary);

   if (!stream.is_open()) {
      SYSTEM_LOG_ERROR("ShaderVulkan::ShaderVulkan: Could not open stream: %s", filename.c_str());
      VT_EXCEPT(std::runtime_error, "ShaderVulkan::ShaderVulkan: Could not open stream!");
   }
   
   size_t fileSize = (size_t)stream.tellg();

   if (fileSize == 0) {
      SYSTEM_LOG_ERROR("ShaderVulkan::ShaderVulkan: File empty: %s", filename.c_str());
   }

   mData.resize(fileSize);
   stream.seekg(0);
   stream.read(mData.data(), fileSize);

   createModule();
}

//-----------------------------------------------------------------
//
VkShaderModule Vt::Gfx::ShaderVulkan::module() const {
   return mModule;
}

//-----------------------------------------------------------------
//
VkPipelineShaderStageCreateInfo Vt::Gfx::ShaderVulkan::stage() const {
   VkPipelineShaderStageCreateInfo shaderStageInfo = {};
   shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
   shaderStageInfo.stage = mStage;
   shaderStageInfo.module = mModule;
   return shaderStageInfo;
}

//-----------------------------------------------------------------
//
Vt::Gfx::ShaderVulkan::~ShaderVulkan() {
   vkDestroyShaderModule(mCtx.vkDevice(), mModule, nullptr);
}

//-----------------------------------------------------------------
//
void Vt::Gfx::ShaderVulkan::createModule() {
   VkShaderModuleCreateInfo moduleCreateInfo{};
   moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   moduleCreateInfo.codeSize = mData.size();
   moduleCreateInfo.pCode = reinterpret_cast<uint32_t*>(mData.data());
   
   VK_CHECK_RESULT(vkCreateShaderModule(mCtx.vkDevice(), &moduleCreateInfo, NULL, &mModule));
}
