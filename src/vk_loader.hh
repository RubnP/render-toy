/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the declaration of the vk_loader class.
 */

#pragma once

#include "ubos.hh"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <optional>
#include <stb_image.h>
#include <vector>
#include <vertex.hh>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct queue_family_indices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool is_complete() {
    return graphics_family.has_value() && present_family.has_value();
  }
};

struct swapchain_support_details {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

class vk_loader {

  //---------------Const-------------------------------
#ifdef NDEBUG
  static constexpr bool M_ENABLE_VALIDATION_LAYERS = false;
#else
  static constexpr bool M_ENABLE_VALIDATION_LAYERS = true;
#endif // Enable validation layers only in debug

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  m_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                   VkDebugUtilsMessageTypeFlagsEXT message_type,
                   const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
                   void *p_user_data) {

    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      std::cerr << "validation layer: " << p_callback_data->pMessage
                << std::endl;
    }

    return VK_FALSE;
  }; // Debug callback to show warning and up error messages

  //---------------Member data-------------------------
  VkInstance m_instance; // Instance

  std::vector<VkPhysicalDevice> m_physical_devices;
  VkPhysicalDevice m_selected_physical_device =
      VK_NULL_HANDLE; // Physical devices

  VkDevice m_logical_device = VK_NULL_HANDLE; // Logical device
  VkQueue m_graphics_queue;
  VkQueue m_present_queue;

  const std::vector<const char *> m_validation_layers = {
      "VK_LAYER_KHRONOS_validation"};

  VkDebugUtilsMessengerEXT m_debug_messenger; // Debug

  const std::vector<const char *> m_device_extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkSurfaceKHR m_surface;
  VkSwapchainKHR m_swapchain;
  std::vector<VkImage> m_swapchain_images;
  std::vector<VkImageView> m_swapchain_image_views;
  std::vector<VkFramebuffer> m_swapchain_framebuffers;
  std::vector<VkCommandBuffer>
      m_command_buffers; // Multiple command buffers for frames in flight

  VkFormat m_swapchain_image_format;
  VkExtent2D m_swapchain_extent;
  uint32_t m_swapchain_image_count;
  VkShaderModule m_def_shader[2];
  VkRenderPass m_render_pass;
  VkDescriptorSetLayout m_descriptor_set_layout;
  VkDescriptorPool m_descriptor_pool;
  std::vector<VkDescriptorSet> m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout;
  VkPipeline m_graphics_pipeline;
  VkCommandPool m_command_pool;

  queue_family_indices m_family_indices;

  VkBuffer m_vertex_buffer;
  VkDeviceMemory m_vertex_buffer_memory;
  VkBuffer m_index_buffer;
  VkDeviceMemory m_index_buffer_memory;

  std::vector<VkBuffer> m_uniform_buffers;
  std::vector<VkDeviceMemory> m_uniform_buffers_memory;
  std::vector<void *> m_uniform_buffers_mapped;

  //---------------Member methods----------------------
  void create_instance();
  bool check_validation_layer_support();
  std::vector<const char *> get_required_extensions(); // Instance

  VkResult create_debug_utils_messenger_ext(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
      const VkAllocationCallbacks *p_allocator,
      VkDebugUtilsMessengerEXT *p_debug_messenger);

  void
  destroy_debug_utils_messenger_ext(VkInstance instance,
                                    VkDebugUtilsMessengerEXT debug_messenger,
                                    const VkAllocationCallbacks *p_allocator);

  void populate_debug_messenger_create_info(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo); // Debug

  bool is_device_suitable(VkPhysicalDevice device);
  bool check_device_extension_support(VkPhysicalDevice device);
  swapchain_support_details query_swapchain_support(VkPhysicalDevice device);
  int rate_physical_device(VkPhysicalDevice device); // Physical devices

  queue_family_indices find_queue_families(VkPhysicalDevice device);
  VkPresentModeKHR choose_swap_present_mode(
      const std::vector<VkPresentModeKHR> &available_present_modes);
  VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities,
                                GLFWwindow *window);
  VkSurfaceFormatKHR choose_swap_surface_format(
      const std::vector<VkSurfaceFormatKHR> available_formats); // Swap chain

  void create_buffer(
      VkDeviceSize size, VkBufferUsageFlags usage,
      VkMemoryPropertyFlags properties, VkBuffer &buffer,
      VkDeviceMemory &buffer_memory); // Helper function for buffer creation

  void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

public:
  //---------------Public methods----------------------
  void init_vulkan();
  void setup_debug_messenger();
  void create_surface(GLFWwindow *window);
  void find_physical_devices();
  void pick_physical_device(uint32_t id = 0);
  void pick_best_physical_device();
  void create_logical_device();
  void create_swapchain(GLFWwindow *window);
  void create_swapchain_image_views();
  void create_render_pass();
  void create_def_graphics_pipeline();
  void create_framebuffers();
  void create_command_pool();
  void create_vertex_buffer(const std::vector<vertex> *vertices);
  void create_index_buffer(const std::vector<uint16_t> *indices);
  void create_command_buffers(const int max_frames_in_flight);
  void create_descriptor_set_layout();
  void create_uniform_buffers(int max_frames_in_flight);
  void create_descriptor_pool();
  void create_descriptor_sets();

  void destroy_vulkan();

  //----------------- Gettors ----------------------------
  VkInstance get_vk_instance();
  VkPhysicalDevice get_selected_physical_device();
  std::vector<VkPhysicalDevice> get_physical_devices();
  VkDevice get_logical_device();
  std::vector<VkCommandBuffer> *get_command_buffers();
  VkRenderPass get_render_pass();
  std::vector<VkFramebuffer> *get_swapchain_framebuffers();
  std::vector<VkImageView> *get_swapchain_image_views();
  VkSwapchainKHR get_swapchain();
  VkExtent2D get_swapchain_extent();
  uint32_t get_swapchain_image_count();
  VkPipeline get_graphics_pipeline();
  VkPipelineLayout get_pipeline_layout();
  VkQueue get_graphics_queue();
  VkQueue get_present_queue();
  queue_family_indices get_queue_family_indices();
  VkBuffer get_vertex_buffer();
  VkBuffer get_index_buffer();
  std::vector<void *> *get_uniform_buffers_mapped();
  std::vector<VkDescriptorSet> *get_descriptor_sets();
  VkDescriptorPool get_descriptor_pool();

  uint32_t find_memory_type(uint32_t type_filter,
                            VkMemoryPropertyFlags property);

  void upload_image_to_gpu(
      glm::ivec3 img_size, stbi_uc *pixels, VkImage *img,
      VkDeviceMemory *img_mem, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
      VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                VK_IMAGE_USAGE_SAMPLED_BIT,
      VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

  VkCommandBuffer begin_single_time_commands();
  void end_single_time_commands(VkCommandBuffer command_buffer);

  void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width,
                            uint32_t height);

  void transition_image_layout(VkImage image, VkFormat format,
                               VkImageLayout old_layout,
                               VkImageLayout new_layout);
};
