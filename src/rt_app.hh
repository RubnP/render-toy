/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief Main loop handler of the application
 */

#pragma once
#include <glm/glm.hpp>
#include <platform/window_manager.hh>
#include <ubos.hh>
#include <vertex.hh>
#include <vk_loader.hh>
#include <vulkan/vulkan_core.h>
class rt_app {

  const std::vector<vertex> m_vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                          {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
  const std::vector<uint16_t> m_indices = {0, 1, 2, 2, 3, 0};

  const int MAX_FRAMES_IN_FLIGHT = 2;
  uint32_t current_frame = 0;
  vk_loader m_vk_loader;
  window_manager m_window_manager;

  std::vector<VkSemaphore> m_image_available_semaphores;
  std::vector<VkSemaphore> m_render_finished_semaphores;
  std::vector<VkFence> m_in_flight_fences;

  void init_window();
  void init_vulkan();
  void main_loop();
  void shutdown();
  void record_command_buffer(VkCommandBuffer command_buffer,
                             uint32_t img_index);
  void create_sync_objects();
  void draw_frame();
  void update_uniform_buffer(uint32_t current_frame);
  void cleanup_swapchain();
  void recreate_swapchain();

public:
  rt_app(){};
  ~rt_app(){};
  void run();
};
