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
		class ShaderVulkan final {
		public:
         ShaderVulkan(const std::vector<char> &buffer);
         ShaderVulkan(const std::ifstream& stream);
         ShaderVulkan(const std::string& filename);
         VkShaderModule getModule() const;

			~ShaderVulkan();
      private:
         std::vector<char> mData;
		};
	}
}
