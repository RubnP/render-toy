/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the declaration of the window manager class
 */
#pragma once

#include <GLFW/glfw3.h>
#include <window.hh>

/**
 * @class
 * @brief This class contains the window management. The main window is the
 * program window, the rest will be dockable imgui windows
 */
class window_manager {
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  platform::window *m_main_window = nullptr;

  // TODO: Create window vector for multiple window support
public:
  void init_window();
  platform::window *get_main_window();
  void update_windows();
  void destroy_window();
};
