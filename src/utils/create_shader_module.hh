/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the create_shader_module funcion. Util functions
 * dont expect usage in a specific context
 */

#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

namespace utils {

/**
 * @brief Creates a shader module based on the provided code
 */

VkShaderModule crete_shader_module(const std::vector<char> &code,
                                   VkDevice logical_device);
} // namespace utils
