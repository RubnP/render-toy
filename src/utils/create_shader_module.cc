/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the implementation of create_shader_module.
 */

#include <create_shader_module.hh>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace utils {

VkShaderModule crete_shader_module(const std::vector<char> &code,
                                   VkDevice logical_device) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shader_module;
  if (vkCreateShaderModule(logical_device, &create_info, nullptr,
                           &shader_module) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }

  return shader_module;
}
} // namespace utils
