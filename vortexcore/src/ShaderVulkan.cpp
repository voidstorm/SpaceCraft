#include "..\include\VortexCore\private\ShaderVulkan.h"


Vt::Gfx::ShaderVulkan::ShaderVulkan(const std::vector<char>& buffer) {

}

Vt::Gfx::ShaderVulkan::ShaderVulkan(const std::ifstream & stream) {

}

Vt::Gfx::ShaderVulkan::ShaderVulkan(const std::string & filename) {

}

VkShaderModule Vt::Gfx::ShaderVulkan::getModule() const {
   return VkShaderModule();
}

Vt::Gfx::ShaderVulkan::~ShaderVulkan() {

}
