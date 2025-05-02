/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the implementation of the vulkan loader class
 */

#include <GLFW/glfw3.h>
#include <cstring>
#include <map>
#include <stdexcept>
#include <vk_loader.hh>
#include <vulkan/vulkan_core.h>

void vk_loader::init_vulkan() { create_instance(); }

/**
 * @brief This function creates the instance of vulkan,
 * TODO: Take the information from actual app information instead of hardcoding
 * it
 */
void vk_loader::create_instance() {

  if (M_ENABLE_VALIDATION_LAYERS && !check_validation_layer_support()) {
    throw std::runtime_error("Validation layers not available");
  } // Check if the validation layers are available

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "render-toy";

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions;

  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  auto extensions = get_required_extensions();
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};

  if (M_ENABLE_VALIDATION_LAYERS) {
    create_info.enabledLayerCount =
        static_cast<uint32_t>(m_validation_layers.size());
    create_info.ppEnabledLayerNames = m_validation_layers.data();

    populate_debug_messenger_create_info(debug_create_info);
    create_info.pNext =
        (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
  } else {
    create_info.enabledLayerCount = 0;

    create_info.pNext = nullptr;
  } // Activate validation layers if requested

  VkResult result = vkCreateInstance(&create_info, nullptr, &m_instance);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to creatae instance");
  }
}
void vk_loader::populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &create_info) {
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = m_debug_callback;
  create_info.pUserData = nullptr;
};

void vk_loader::setup_debug_messenger() {
  if (!M_ENABLE_VALIDATION_LAYERS)
    return;

  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  populate_debug_messenger_create_info(create_info);

  if (create_debug_utils_messenger_ext(m_instance, &create_info, nullptr,
                                       &m_debug_messenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger");
  }
}

VkInstance vk_loader::get_vk_instance() { return m_instance; }
VkPhysicalDevice vk_loader::get_selected_physical_device() {
  return m_selected_physical_device;
}
std::vector<VkPhysicalDevice> vk_loader::get_physical_devices() {
  return m_physical_devices;
}

/**
 * @brief This function will return true if and only if all the validation layer
 * specified in m_validation_layers are available
 */
bool vk_loader::check_validation_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);

  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char *layer_name : m_validation_layers) {
    bool layer_found = false;
    for (const auto &layer_properties : available_layers) {
      if (std::strcmp(layer_name, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      return false; // Layer not found, we cannot activate validation layers :<
      // TODO: Create a better system that returns a bitset so all the available
      // validation layers can be activated (instead of all of none)
    }
  } // Check if every validation layer actually exists in the available layers
  return true;
}

std::vector<const char *> vk_loader::get_required_extensions() {
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  std::vector<const char *> extensions(glfw_extensions,
                                       glfw_extensions + glfw_extension_count);
  if (M_ENABLE_VALIDATION_LAYERS) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  return extensions;
}

/**
 * @brief Creates the debug messenger
 */
VkResult vk_loader::create_debug_utils_messenger_ext(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, p_create_info, p_allocator, p_debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

/**
 * @brief Destroys the debug messenger
 */
void vk_loader::destroy_debug_utils_messenger_ext(
    VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debug_messenger, p_allocator);
  }
}

bool vk_loader::is_device_suitable(VkPhysicalDevice device) {
  // TODO: Once the app is more complete actually implement this function with
  // the requirements of the pipeline created by the user

  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceProperties(device, &device_properties);
  vkGetPhysicalDeviceFeatures(device, &device_features);

  return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
         device_features.geometryShader;
}

/**
 * @brief Find all the physical devices and stores them in the internal list
 */
void vk_loader::find_physical_devices() {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
  if (device_count == 0) {
    throw std::runtime_error("Failed to find GPU with vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

  m_physical_devices.clear();
  for (const auto &device : devices) {
    if (is_device_suitable(device)) {
      m_physical_devices.push_back(device);
    }
  }

  if (m_physical_devices.empty()) {
    throw std::runtime_error("Failed to find a GPU with required features");
  }
}

/**
 * @brief Change the physical device to use.
 */
void vk_loader::pick_physical_device(uint32_t id) {
  m_selected_physical_device = m_physical_devices[id];
}

void vk_loader::pick_best_physical_device() {
  // Use an ordered map to automatically sort candidates by increasing score
  std::multimap<int, VkPhysicalDevice> candidates;

  for (const auto &device : m_physical_devices) {
    int score = rate_physical_device(device);
    candidates.insert(std::make_pair(score, device));
  }

  // Check if the best candidate is suitable at all
  if (candidates.rbegin()->first > 0) {
    m_selected_physical_device = candidates.rbegin()->second;
  } else {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

int vk_loader::rate_physical_device(VkPhysicalDevice device) {

  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceProperties(device, &device_properties);
  vkGetPhysicalDeviceFeatures(device, &device_features);

  int score = 0;

  // Discrete GPUs have a significant performance advantage
  if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  // Maximum possible size of textures affects graphics quality
  score += device_properties.limits.maxImageDimension2D;

  // Application can't function without geometry shaders
  if (!device_features.geometryShader) {
    return 0;
  }

  return score;
}

void vk_loader::destroy_vulkan() {
  if (M_ENABLE_VALIDATION_LAYERS) {
    destroy_debug_utils_messenger_ext(m_instance, m_debug_messenger, nullptr);
  }
  vkDestroyInstance(m_instance, nullptr);
}
