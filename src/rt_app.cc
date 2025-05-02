/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief Main loop handler implementation
 */

#include <rt_app.hh>

void rt_app::run() {
  init_window();
  init_vulkan();
  main_loop();
  shutdown();
}

void rt_app::init_window() { m_window_manager.init_window(); }

void rt_app::init_vulkan() {
  m_vk_loader.init_vulkan();
  m_vk_loader.setup_debug_messenger();
}

void rt_app::main_loop() {
  while (!glfwWindowShouldClose(m_window_manager.get_main_window())) {
    glfwPollEvents();
  }
}

void rt_app::shutdown() {
  m_window_manager.destroy_window();
  m_vk_loader.destroy_vulkan();
}
