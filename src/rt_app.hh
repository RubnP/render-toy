/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief Main loop handler of the application
 */

#pragma once
#include <platform/window_manager.hh>
#include <vk_loader.hh>
#include <vulkan/vulkan_core.h>

class rt_app {
  vk_loader m_vk_loader;
  window_manager m_window_manager;

  VkSemaphore m_image_available_semaphore;
  VkSemaphore m_render_finished_semaphore;
  VkFence m_in_fight_fence;

  void init_window();
  void init_vulkan();
  void main_loop();
  void shutdown();
  void record_command_buffer(VkCommandBuffer command_buffer,
                             uint32_t img_index);
  void create_sync_objects();
  void draw_frame();

public:
  void run();
};
