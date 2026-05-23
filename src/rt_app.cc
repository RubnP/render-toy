/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief Main loop handler implementation
 */

#include <cstdint>
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
  m_vk_loader.create_swapchain(m_window_manager.get_main_window());
  m_vk_loader.create_swapchain_image_views();
  m_vk_loader.create_render_pass();
  m_vk_loader.create_def_graphics_pipeline();
  m_vk_loader.create_framebuffers();
  m_vk_loader.create_command_pool();
  m_vk_loader.create_command_buffers(MAX_FRAMES_IN_FLIGHT);
  create_sync_objects();
}

void rt_app::main_loop() {
  while (!glfwWindowShouldClose(m_window_manager.get_main_window())) {
    glfwPollEvents();
    draw_frame();
  }

  vkDeviceWaitIdle(m_vk_loader.get_logical_device());
}

void rt_app::shutdown() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    vkDestroySemaphore(m_vk_loader.get_logical_device(),
                       m_render_finished_semaphores[i], nullptr);
    vkDestroySemaphore(m_vk_loader.get_logical_device(),
                       m_image_available_semaphores[i], nullptr);
    vkDestroyFence(m_vk_loader.get_logical_device(), m_in_flight_fences[i],
                   nullptr);
    m_window_manager.destroy_window();
    m_vk_loader.destroy_vulkan();
  }
}

void rt_app::create_sync_objects() {

  m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

    if (vkCreateSemaphore(m_vk_loader.get_logical_device(), &semaphore_info,
                          nullptr,
                          &m_image_available_semaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(m_vk_loader.get_logical_device(), &semaphore_info,
                          nullptr,
                          &m_render_finished_semaphores[i]) != VK_SUCCESS ||
        vkCreateFence(m_vk_loader.get_logical_device(), &fence_info, nullptr,
                      &m_in_flight_fences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores!");
    }
  }
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
      m_vk_loader.get_swapchain_framebuffers()->at(img_index);

  render_pass_info.renderArea.offset = {0, 0};
  VkExtent2D swapchain_extent = m_vk_loader.get_swapchain_extent();
  render_pass_info.renderArea.extent = swapchain_extent;

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
  viewport.width = static_cast<float>(swapchain_extent.width);
  viewport.height = static_cast<float>(swapchain_extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(command_buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapchain_extent;
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);

  vkCmdDraw(command_buffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(command_buffer);

  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}

void rt_app::draw_frame() {
  VkDevice device = m_vk_loader.get_logical_device();
  vkWaitForFences(device, 1, &m_in_flight_fences[current_frame], VK_TRUE,
                  UINT64_MAX);
  vkResetFences(device, 1, &m_in_flight_fences[current_frame]);

  uint32_t img_index;
  vkAcquireNextImageKHR(device, m_vk_loader.get_swapchain(), UINT64_MAX,
                        m_image_available_semaphores[current_frame],
                        VK_NULL_HANDLE, &img_index);
  vkResetCommandBuffer(m_vk_loader.get_command_buffers()[current_frame], 0);
  record_command_buffer(m_vk_loader.get_command_buffers()[current_frame],
                        img_index);

  // Submit the recorded command buffer to draw

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_semaphores[] = {m_image_available_semaphores[current_frame]};
  VkPipelineStageFlags wait_stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;

  submit_info.commandBufferCount = 1;
  VkCommandBuffer command_buffer =
      m_vk_loader.get_command_buffers()[current_frame];
  submit_info.pCommandBuffers = &command_buffer;

  VkSemaphore signal_semaphores[] = {
      m_render_finished_semaphores[current_frame]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;
  if (vkQueueSubmit(m_vk_loader.get_graphics_queue(), 1, &submit_info,
                    m_in_flight_fences[current_frame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer");
  }

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapchains[] = {m_vk_loader.get_swapchain()};
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &img_index;

  present_info.pResults = nullptr;
  vkQueuePresentKHR(m_vk_loader.get_present_queue(), &present_info);

  current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT; // loop the frames
}
