/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains the declaration for the vertex and some helper
 * functions
 */

#pragma once
#include <array>
#include <cstddef>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

struct vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static VkVertexInputBindingDescription get_binding_description() {
    VkVertexInputBindingDescription binding_description{};
    binding_description.binding = 0;
    binding_description.stride = sizeof(vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
  }

  static std::array<VkVertexInputAttributeDescription, 2>
  get_attribute_descriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{};

    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(vertex, pos);

    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(vertex, color);

    return attribute_descriptions;
  }
};
