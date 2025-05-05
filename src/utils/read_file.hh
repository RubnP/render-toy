/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the read_file funcion. Util functions dont expect
 * usage in a specific context
 */

#pragma once
#include <string>
#include <vector>

namespace utils {

/**
 * @brief Reads a file as a binary (vector of chars). Takes the relative path to
 * the file as a string
 */

std::vector<char> read_file(const std::string &file_name);
} // namespace utils
