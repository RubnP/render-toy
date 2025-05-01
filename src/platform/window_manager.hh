/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the declaration of the window manager class
 */
#pragma once

#include <GLFW/glfw3.h>

/**
 * @class
 * @brief This class contains the window management. The main window is the
 * program window, the rest will be dockable imgui windows
 */
class window_manager {
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  GLFWwindow *m_main_window = nullptr; // Main program window
public:
  void init_window();
  GLFWwindow *get_main_window();
  void destroy_window();
};
