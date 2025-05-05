/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the create_shader_module funcion. Util functions
 * dont expect usage in a specific context
 */

#pragma once
#include <vector>

namespace utils {

/**
 * @brief Creates a shader module based on the provided code
 */

std::vector<char> crete_shader_module(const std::vector<char> &code);
} // namespace utils
