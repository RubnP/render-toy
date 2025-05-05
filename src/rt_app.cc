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
  m_vk_loader.create_surface(m_window_manager.get_main_window());
  m_vk_loader.find_physical_devices();
  m_vk_loader.pick_best_physical_device(); // TODO: Make something more fancy
                                           // than selecting the first
                                           // compatible GPU. A menu for the
                                           // user to select once in the app?
  m_vk_loader.create_logical_device();
  m_vk_loader.create_swap_chain(m_window_manager.get_main_window());
  m_vk_loader.create_swap_chain_image_views();
  m_vk_loader.create_graphics_pipeline();
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
