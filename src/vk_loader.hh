/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the declaration of the vk_loader class.
 */

#pragma once

#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

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
  VkInstance m_instance;

  const std::vector<const char *> m_validation_layers = {
      "VK_LAYER_KHRONOS_validation"};

  VkDebugUtilsMessengerEXT m_debug_messenger;

  //---------------Member methods----------------------
  void create_instance();
  bool check_validation_layer_support();
  std::vector<const char *> get_required_extensions();

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
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

public:
  //---------------Public methods----------------------
  void init_vulkan();
  void setup_debug_messenger();
  VkInstance *get_vk_instance();
  void destroy_vulkan();
};
