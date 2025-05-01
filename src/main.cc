/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 */

#include <exception>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <rt_app.hh>

#include <iostream>

/**
 * @brief Entry of the program
 */
int main(void) {
  rt_app app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
