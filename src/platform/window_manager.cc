/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the implementation of the window manager
 */

#include "window.hh"
#include <GLFW/glfw3.h>
#include <imgui_hnd.hh>
#include <window_manager.hh>

void window_manager::init_window() {
  m_main_window = new platform::window(WIDTH, HEIGHT);
}

platform::window *window_manager::get_main_window() { return m_main_window; }

void window_manager::destroy_window() {
  glfwDestroyWindow(m_main_window->get_window());
  glfwTerminate();

  delete m_main_window;
  m_main_window = nullptr;
}

void window_manager::update_windows() {
  // TODO: Eventually change this to update the vector of windows
  m_main_window->update();
}
