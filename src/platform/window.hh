/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the definition for the window class
 */
#pragma once

#include <GLFW/glfw3.h>
#include <functional>

#define WINDOW_FUNCTION std::function<void()>

namespace platform {
/**
 * @class
 * @brief The window class encapsulates the window utilities
 * Windows are based on ImGui Windows. Windows gain functionality through the
 * usage of lamnda functions that utilize ImGui functionality
 */
class window {
  std::vector<WINDOW_FUNCTION>
      m_functions; // ImGui functions that exist in the window

public:
  window();
  window(const std::vector<WINDOW_FUNCTION> &funcs);
  void add_functions(const std::vector<WINDOW_FUNCTION> &funcs);
  void add_functions(const WINDOW_FUNCTION &func);
  void update();
  ~window();
};
} // namespace platform
