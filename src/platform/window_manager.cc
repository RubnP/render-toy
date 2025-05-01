/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the implementation of the window manager
 */

#include <GLFW/glfw3.h>
#include <window_manager.hh>

void window_manager::init_window() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_main_window =
      glfwCreateWindow(WIDTH, HEIGHT, "render-toy", nullptr, nullptr);
}

GLFWwindow *window_manager::get_main_window() { return m_main_window; }

void window_manager::destroy_window() {
  glfwDestroyWindow(m_main_window);
  glfwTerminate();
}
