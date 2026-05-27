#include "vk_loader.hh"
#include <cstdint>
#include <imgui_hnd.hh>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_core.h>

// TODO: Remove the returns once we hace DescriptorPools

static VkDescriptorPool imgui_pool;
void imgui_hnd::imgui_init(vk_loader *vk_ld_ptr, platform::window *wnd) {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;            // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch


  ImGui_ImplGlfw_InitForVulkan(wnd->get_window(), true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vk_ld_ptr->get_vk_instance();
  init_info.PhysicalDevice = vk_ld_ptr->get_selected_physical_device();
  init_info.Device = vk_ld_ptr->get_logical_device();
  init_info.QueueFamily =
      vk_ld_ptr->get_queue_family_indices().graphics_family.value();
  init_info.Queue = vk_ld_ptr->get_graphics_queue();
  // init_info.PipelineCache = vk_ld_ptr->get_graphics_pipeline();
  //

  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount = IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;

  vkCreateDescriptorPool(vk_ld_ptr->get_logical_device(), &pool_info, nullptr,
                         &imgui_pool);
  init_info.DescriptorPool = imgui_pool; // TODO: Create descriptor

  // pool for uniform buffer and put it here.
  init_info.MinImageCount = vk_ld_ptr->get_swapchain_image_count();
  init_info.ImageCount = vk_ld_ptr->get_swapchain_image_count();
  // init_info.Allocator = YOUR_ALLOCATOR;
  init_info.RenderPass = vk_ld_ptr->get_render_pass();
  init_info.Subpass = 0;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  // init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info);
}

void imgui_hnd::imgui_main_loop_start() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void imgui_hnd::imgui_main_loop_end() {
  // Finish ImGui frame and prepare draw data. Actual submission of the
  // draw-data must be recorded into an active VkCommandBuffer while a render
  // pass is active. The caller is responsible for invoking
  // ImGui_ImplVulkan_RenderDrawData() during command buffer recording.
  ImGui::Render();
}

void imgui_hnd::imgui_shutdown(vk_loader *vk_ld_ptr) {
  vkDestroyDescriptorPool(vk_ld_ptr->get_logical_device(), imgui_pool, nullptr);
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
