/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief Main loop handler implementation
 */

#include <rt_app.hh>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

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
  m_vk_loader.create_render_pass();
  m_vk_loader.create_def_graphics_pipeline();
  m_vk_loader.create_framebuffers();
  m_vk_loader.create_command_pool();
  m_vk_loader.create_command_buffer();
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

//---------------Drawing functions
void rt_app::record_command_buffer(VkCommandBuffer command_buffer,
                                   uint32_t img_index) {
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer");
  }

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = m_vk_loader.get_render_pass();
  render_pass_info.framebuffer =
      m_vk_loader.get_swap_chain_framebuffers()->at(img_index);

  render_pass_info.renderArea.offset = {0, 0};
  VkExtent2D swap_chain_extent = m_vk_loader.get_swap_chain_extent();
  render_pass_info.renderArea.extent = swap_chain_extent;

  VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_color;

  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    m_vk_loader.get_graphics_pipeline());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swap_chain_extent.width);
  viewport.height = static_cast<float>(swap_chain_extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(command_buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swap_chain_extent;
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);

  vkCmdDraw(command_buffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(command_buffer);

  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}
