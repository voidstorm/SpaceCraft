#pragma once
#include <memory>
#include <atomic>
#include <vector>
#include <exception>
#include "vulkan\vulkan.h"

#include "..\SystemLogger.h"
#include "VkErrorHelper.h"
#include <mutex>

namespace Vt {
namespace Gfx {
		class ShaderVulkan {
		public:
         ShaderVulkan();
			virtual ~ShaderVulkan();
		};
	}
}
