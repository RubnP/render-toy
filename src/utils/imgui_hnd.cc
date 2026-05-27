#include "vk_loader.hh"
#include <cstdint>
#include <imgui_hnd.hh>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// TODO: Remove the returns once we hace DescriptorPools

void imgui_hnd::imgui_init(vk_loader *vk_ld_ptr, platform::window *wnd) {
  return;
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;            // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForVulkan(wnd->get_window(), false);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vk_ld_ptr->get_vk_instance();
  init_info.PhysicalDevice = vk_ld_ptr->get_selected_physical_device();
  init_info.Device = vk_ld_ptr->get_logical_device();
  init_info.QueueFamily =
      vk_ld_ptr->get_queue_family_indices().graphics_family.value();
  init_info.Queue = vk_ld_ptr->get_graphics_queue();
  // init_info.PipelineCache = vk_ld_ptr->get_graphics_pipeline();
  init_info.DescriptorPool =
      vk_ld_ptr->get_descriptor_pool(); // TODO: Create descriptor
  // pool for uniform buffer and put it here.
  init_info.MinImageCount = 2;
  init_info.ImageCount = 2;
  // init_info.Allocator = YOUR_ALLOCATOR;
  init_info.RenderPass = vk_ld_ptr->get_render_pass();
  init_info.Subpass = 0;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  // init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info);
}

void imgui_hnd::imgui_main_loop_start() {
  return;
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void imgui_hnd::imgui_main_loop_end(vk_loader *vk_ld_ptr, uint32_t c_frame) {
  return;
  // Rendering
  // (Your code clears your framebuffer, renders your other stuff etc.)
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(
      ImGui::GetDrawData(), vk_ld_ptr->get_command_buffers()->at(c_frame));
  // (Your code calls vkCmdEndRenderPass, vkQueueSubmit, vkQueuePresentKHR etc.)
}

void imgui_hnd::imgui_shutdown() {
  return;
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
