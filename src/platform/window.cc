/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the implementation of the window class
 */

#include <vector>
#include <window.hh>

using namespace platform;

/**
 * @brief def initialization
 */
window::window(int WIDTH, int HEIGHT) {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  m_window = glfwCreateWindow(WIDTH, HEIGHT, "render-toy", nullptr, nullptr);
};

window::~window() {}

/**
 * @brief initialization with content
 */
window::window(const std::vector<WINDOW_FUNCTION> &funcs)
    : m_functions(funcs) {}

/**
 * @brief Add functionality to the window (multiple functions overload)
 */
void window::add_functions(const std::vector<WINDOW_FUNCTION> &funcs) {
  for (auto &fn : funcs) {
    m_functions.push_back(fn);
  }
}

/**
 * @brief Add functionality to the window (single function overload)
 */
void window::add_functions(const WINDOW_FUNCTION &func) {
  m_functions.push_back(func);
}

/**
 * @bief Update the window based on the given functionality
 */

void window::update() {
  for (auto &fn : m_functions) {
    fn(); // ImGui behaviour
  }
}

GLFWwindow *window::get_window() { return m_window; }
