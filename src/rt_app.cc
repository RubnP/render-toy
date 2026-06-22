/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief Main loop handler implementation
 */

#include "imgui.h"
#include "vk_loader.hh"
#include <cstdint>
#include <imgui_hnd.hh>
#include <imgui_impl_vulkan.h>
#include <rt_app.hh>
#include <stdexcept>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#define GLM_FORCE_RADIANS
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

void rt_app::run() {
  init_window();
  init_vulkan();

  // Lets add imgui functionality to this fist window TODO: Move this to a
  // better place
  imgui_hnd::imgui_init(&m_vk_loader, m_window_manager.get_main_window());

  main_loop();
  shutdown();
}

void rt_app::init_window() {
  m_window_manager.init_window();
  m_window_manager.get_main_window()->add_functions(
      []() { ImGui::ShowDemoWindow(); });
}

void rt_app::init_vulkan() {
  m_vk_loader.init_vulkan();
  m_vk_loader.setup_debug_messenger();
  m_vk_loader.create_surface(m_window_manager.get_main_window()->get_window());
  m_vk_loader.find_physical_devices();
  m_vk_loader.pick_best_physical_device(); // TODO: Make something more fancy
                                           // than selecting the first
                                           // compatible GPU. A menu for the
                                           // user to select once in the app?
  m_vk_loader.create_logical_device();
  m_vk_loader.create_swapchain(
      m_window_manager.get_main_window()->get_window());
  m_vk_loader.create_swapchain_image_views();
  m_vk_loader.create_render_pass();
  m_vk_loader.create_descriptor_set_layout();
  m_vk_loader.create_def_graphics_pipeline();
  m_vk_loader.create_framebuffers();
  m_vk_loader.create_command_pool();
  create_texture_image();
  create_texture_image_view();
  create_texture_sampler();
  m_vk_loader.create_vertex_buffer(&m_vertices);
  m_vk_loader.create_index_buffer(&m_indices);
  m_vk_loader.create_uniform_buffers(MAX_FRAMES_IN_FLIGHT);
  m_vk_loader.create_descriptor_pool();
  m_vk_loader.create_descriptor_sets(m_def_tex_image_view, m_def_tex_sampler);
  m_vk_loader.create_command_buffers(MAX_FRAMES_IN_FLIGHT);
  create_sync_objects();
}

void rt_app::main_loop() {
  while (!glfwWindowShouldClose(
      m_window_manager.get_main_window()->get_window())) {

    glfwPollEvents();

    imgui_hnd::imgui_main_loop_start();
    m_window_manager.update_windows();

    // Finish ImGui frame and produce draw data so it can be recorded into the
    // command buffer (draw_frame records ImGui draw data into the active
    // command buffer while the render pass is open).
    imgui_hnd::imgui_main_loop_end();

    draw_frame();
  }

  vkDeviceWaitIdle(m_vk_loader.get_logical_device());
}

void rt_app::shutdown() {
  imgui_hnd::imgui_shutdown(&m_vk_loader);

  cleanup_swapchain();

  vkDestroySampler(m_vk_loader.get_logical_device(), m_def_tex_sampler,
                   nullptr);
  vkDestroyImageView(m_vk_loader.get_logical_device(), m_def_tex_image_view,
                     nullptr); // Destroy default texture, TODO: Eventually
                               // clean all textures on the GPU on shutdown

  vkDestroyImage(m_vk_loader.get_logical_device(), m_def_tex, nullptr);
  vkFreeMemory(m_vk_loader.get_logical_device(), m_def_tex_mem, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    vkDestroySemaphore(m_vk_loader.get_logical_device(),
                       m_render_finished_semaphores[i], nullptr);
    vkDestroySemaphore(m_vk_loader.get_logical_device(),
                       m_image_available_semaphores[i], nullptr);
    vkDestroyFence(m_vk_loader.get_logical_device(), m_in_flight_fences[i],
                   nullptr);
  }
  m_vk_loader.destroy_vulkan();
  m_window_manager.destroy_window();
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

  VkBuffer vertex_buffers[] = {m_vk_loader.get_vertex_buffer()};
  VkDeviceSize offsets_vertex[] = {0};
  vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets_vertex);
  vkCmdBindIndexBuffer(command_buffer, m_vk_loader.get_index_buffer(), 0,
                       VK_INDEX_TYPE_UINT16);

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

  vkCmdBindDescriptorSets(
      command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      m_vk_loader.get_pipeline_layout(), 0, 1,
      &(m_vk_loader.get_descriptor_sets()->at(current_frame)), 0, nullptr);

  // vkCmdDraw(command_buffer, 3, 1, 0, 0);
  vkCmdDrawIndexed(command_buffer, m_indices.size(), 1, 0, 0, 0);

  // Render ImGui draw data into the same command buffer while the render
  // pass is active. imgui_hnd::imgui_main_loop_end() must have been called
  // earlier to produce draw data via ImGui::Render(). We now submit it here.
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);

  vkCmdEndRenderPass(command_buffer);

  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}

void rt_app::draw_frame() {

  VkDevice device = m_vk_loader.get_logical_device();
  vkWaitForFences(device, 1, &m_in_flight_fences[current_frame], VK_TRUE,
                  UINT64_MAX);

  uint32_t img_index;
  VkResult result = vkAcquireNextImageKHR(
      device, m_vk_loader.get_swapchain(), UINT64_MAX,
      m_image_available_semaphores[current_frame], VK_NULL_HANDLE, &img_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    recreate_swapchain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swapchain image");
  }

  vkResetFences(device, 1, &m_in_flight_fences[current_frame]);

  vkResetCommandBuffer(m_vk_loader.get_command_buffers()->at(current_frame), 0);
  record_command_buffer(m_vk_loader.get_command_buffers()->at(current_frame),
                        img_index);

  // Update the uniforms
  update_uniform_buffer(current_frame);

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
      m_vk_loader.get_command_buffers()->at(current_frame);
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

void rt_app::cleanup_swapchain() {
  for (auto framebuffer : *m_vk_loader.get_swapchain_framebuffers()) {
    vkDestroyFramebuffer(m_vk_loader.get_logical_device(), framebuffer,
                         nullptr);
  }

  for (auto image_view : *m_vk_loader.get_swapchain_image_views()) {
    vkDestroyImageView(m_vk_loader.get_logical_device(), image_view, nullptr);
  }

  vkDestroySwapchainKHR(m_vk_loader.get_logical_device(),
                        m_vk_loader.get_swapchain(), nullptr);
}

void rt_app::recreate_swapchain() {
  vkDeviceWaitIdle(m_vk_loader.get_logical_device());

  cleanup_swapchain();

  m_vk_loader.create_swapchain(
      m_window_manager.get_main_window()->get_window());
  m_vk_loader.create_swapchain_image_views();
  m_vk_loader.create_framebuffers();
}

void rt_app::update_uniform_buffer(uint32_t current_frame) {
  static auto start_time = std::chrono::high_resolution_clock::now();
  auto current_time = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   current_time - start_time)
                   .count();

  trans_mat trans;
  trans.mod_world = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));
  trans.world_cam =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));

  trans.proj =
      glm::perspective(glm::radians(45.0f),
                       m_vk_loader.get_swapchain_extent().width /
                           (float)m_vk_loader.get_swapchain_extent().height,
                       0.1f, 10.0f);

  trans.proj[1][1] *= -1;

  memcpy(m_vk_loader.get_uniform_buffers_mapped()->at(current_frame), &trans,
         sizeof(trans));
}

void rt_app::create_texture_image() {
  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load("data/def_texture.jpg", &tex_width, &tex_height,
                              &tex_channels, STBI_rgb_alpha);

  if (!pixels) {
    throw std::runtime_error("Could not load the default texture");
  }

  m_vk_loader.upload_image_to_gpu({tex_width, tex_height, tex_channels}, pixels,
                                  &m_def_tex, &m_def_tex_mem);

  stbi_image_free(pixels);
}

void rt_app::create_texture_image_view() {
  m_def_tex_image_view =
      m_vk_loader.create_image_view(m_def_tex, VK_FORMAT_R8G8B8A8_SRGB);
}

void rt_app::create_texture_sampler() {
  VkSamplerCreateInfo sampler_info{};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;

  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  sampler_info.anisotropyEnable = VK_TRUE;
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(m_vk_loader.get_selected_physical_device(),
                                &properties);
  sampler_info.maxAnisotropy =
      properties.limits.maxSamplerAnisotropy; // TODO: Make this a toggle

  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;

  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;

  if (vkCreateSampler(m_vk_loader.get_logical_device(), &sampler_info, nullptr,
                      &m_def_tex_sampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler");
  }
}
